/************************************************************************
* This file is part of AniMiku.											*
*																		*
* AniMiku is free software : you can redistribute it and / or modify	*
* it under the terms of the GNU General Public License as published by	*
* the Free Software Foundation, either version 3 of the License, or		*
* (at your option) any later version.									*
*																		*
* AniMiku is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of		*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the			*
* GNU General Public License for more details.							*
*																		*
* You should have received a copy of the GNU General Public License		*
* along with AniMiku. If not, see <http://www.gnu.org/licenses/>.		*
*																		*
* AniMiku is Copyright (c) 2014 Justin Byers							*
*************************************************************************/

#define DX10 1
//NEW MARK
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <time.h>

#include <d3d10_1.h>
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\D3DX10.h>
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\DxErr.h>

#include "AM_Utils.h"


#define NAME_SZ 20
#define MAX_BUFLEN 2048
#define BUILD_VERSION  4.000f
#define MOTION_FPS 60

#ifndef _TYPES
#define _TYPES
typedef struct _vec3 {
	float x;
	float y;
	float z;
} vec3;

typedef enum WeightType{BDEF1=0,BDEF2,BDEF4,SDEF};
typedef enum SphereMode{NoSphere=0,Multiply,Add,Subtract};
struct MMDVERTEX
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 uv;
	D3DXVECTOR4 boneIndices;
	D3DXVECTOR4 boneWeights;

	//random PMX stuff that makes no sense
	D3DXVECTOR4 *addingUV;
	WeightType weightType;
	D3DXVECTOR3 SDSF_C;
	D3DXVECTOR3 SDSF_R0;
	D3DXVECTOR3 SDSF_R1;


	D3DXMATRIX curFaceTrans, curFaceTransUV;
	D3DXVECTOR3 origPos;
	D3DXVECTOR3 origNormal;
	D3DXVECTOR2 origUV;
};

#pragma pack(push, 1)

enum PMD_BONE_TYPE {
   ROTATE,          /* normal bone, can rotate by motion */
   ROTATE_AND_MOVE, /* normal bone, can rotate and move by motion */
   IK_DESTINATION,  /* IK bone, can move by motion */
   UNKNOWN,         /* unknown */
   UNDER_IK,        /* controlled under an IK chain */
   UNDER_ROTATE,    /* controlled by other bone: rotation copied from target bone */
   IK_TARGET,       /* IK target bone, directly manipulated by IK toward an IK destination */
   NO_DISP,         /* no display (bone end markers) */
   TWIST,           /* allow twisting rotation only */
   FOLLOW_ROTATE    /* follow the child bone's rotation by the specified rate */
};

typedef struct _texCoord {
	float u;
	float v;
} texCoord;
typedef struct _PMD_Header {
   char id[3];     
   float version;     
   char name[20];     
   char comment[256]; 
} PMD_Header;

typedef struct _PMD_Vertex {
   float pos[3];              /* position (x, y, z) */
   float normal[3];           /* normal vector (x, y, z) */
   float uv[2];               /* texture coordinate (u, v) */
   short boneID[2];           /* bone IDs for skinning */
   unsigned char boneWeight1; /* weight for boneID[0], (1-boneWeight1) for boneID[1] */
   unsigned char noEdgeFlag;  /* 1 if NO edge should be drawn for this vertex */
} PMD_Vertex;

typedef struct _PMD_Material {
   float diffuse[3];              /* diffuse color */
   float alpha;                   /* alpha color */
   float shiness;                 /* shiness intensity */
   float specular[3];             /* specular color */
   float ambient[3];              /* ambient color */
   unsigned char toonID;          /* toon index: 0xff -> toon0.bmp, other -> toon(val+1).bmp */
   unsigned char edgeFlag;        /* 1 if edge should be drawn */
   unsigned long numSurfaceIndex; /* number of surface indices for this material */
   char textureFile[20];          /* texture file name */
} PMD_MaterialI;

typedef struct _PMD_Bone {
   char name[20];      /* bone name */
   short parentBoneID; /* parent bone ID (-1 = none) */
   short childBoneID;  /* child bone ID (-1 = none) */
   unsigned char type; /* bone type (PMD_BONE_TYPE) */
   short targetBoneID; /* bone ID by which this bone is affected: only for IK bone (type 4), under_rotate bone (type 5) or co-rotate coef value (type 9) */
   float pos[3];       /* position from origin */
} PMD_Bone;

typedef struct _PMD_IK {
   short destBoneID;            /* bone of destination position */
   short targetBoneID;          /* bone to be directly manipulated by this IK */
   unsigned char numLink;       /* length of bone list affected by this IK */
   unsigned short numIteration; /* number of maximum iteration (IK value 1) */
   float angleConstraint;       /* maximum angle per IK step in radian (IK value 2) */
} PMD_IK;

typedef struct _PMD_FACE {
	char name[20];
	unsigned long numVertex;
	unsigned char type;
} PMD_FACE;

typedef struct _PMD_FACE_VERTEX {
	unsigned long vertexID;
	float pos[3];
} PMD_FACE_VERTEX;

typedef struct _PMD_RigidBody {
   char name[20];                  /* name of this rigid body (unused) */
   unsigned short boneID;          /* related bone */
   unsigned char collisionGroupID; /* collision group in which this body belongs to */
   unsigned short collisionMask;   /* collision group mask */
   unsigned char shapeType;        /* shape type (0: sphere, 1: box, 2: capsule) */
   float width;                    /* width of this shape (valid for 0, 1, 2) */
   float height;                   /* height of this shape (valid for 1, 2) */
   float depth;                    /* depth of this shape (valied for 1) */
   float pos[3];                   /* position (x, y, z), relative to related bone */
   float rot[3];                   /* rotation (x, y, z), in radian */
   float mass;                     /* weight (valid for type 1 and 2) */
   float linearDamping;            /* linear damping coefficient */
   float angularDamping;           /* angular damping coefficient */
   float restitution;              /* restitution (recoil) coefficient */
   float friction;                 /* friction coefficient */
   unsigned char type;             /* type (0: kinematics, 1: simulated, 2: simulated+aligned) */
} PMD_RigidBody;

/* Bulletphysics Constraint element */
typedef struct _PMD_Constraint {
   char name[20];         /* name of this constraint (unused) */
   unsigned long bodyIDA; /* ID of body A */
   unsigned long bodyIDB; /* ID of body B */
   float pos[3];          /* position (x, y, z), relative to related bone */
   float rot[3];          /* rotation (x, y, z), in radian */
   float limitPosFrom[3]; /* position move limit from (x, y, z) */
   float limitPosTo[3];   /* position move limit to (x, y, z) */
   float limitRotFrom[3]; /* rotation angular limit from (x, y, z) (rad) */
   float limitRotTo[3];   /* rotation angular limit to (x, y, z) (rad) */
   float stiffness[6];    /* spring stiffness (x,y,z,rx,ry,rz) */
} PMD_Constraint;




typedef struct _VMD_Header {
   char header[30]; /* "Vocaloid Motion Data 0002" */
   char name[20];   /* model name (unused) */
} VMD_Header;

/* VMDFile_BoneFrame: bone motion element structure for VMD file reading */
typedef struct _VMD_BoneFrame {
   char name[15];          /* bone name */
   unsigned long keyFrame; /* key frame */
   float pos[3];           /* position (x, y, z) */
   float rot[4];           /* rotation (x, y, z, w) */
   char interpolation[64];
   /*char intX[16]; 
   char intY[16];
   char intZ[16];
   char intRot[16];*/
} VMD_BoneFrame;

typedef struct _VMD_FaceFrame {
   char name[15];          /* face name */
   unsigned long keyFrame; /* key frame */
   float weight;           /* weight (0.0 - 1.0) */
} VMD_FaceFrame;

typedef struct _PMX_HEADER {
	char id[4];
	float version;
} PMX_HEADER;
typedef struct _PMX_INFO{
	wchar_t name[12];
} PMX_INFO;
#pragma pack(pop)
#endif