/************************************************************************
*	AM_Morph.h -- Morph management and calculation						*
*				  Copyright (c) 2014 Justin Byers						*
* ===================================================================== *
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
*************************************************************************/

#ifndef MORPH_H_
#define MORPH_H_

#include "AMTypes.h"
#include "AM_BoneNew.h"
#include "PMD_Textures.h"

//types used in morphs
struct MorphDefs
{
	typedef enum MorphCategory{SystemReserved=0,Eyebrow,Eye,Mouth,Other};
	typedef enum MorphType{Group=0,Vertex,Bone,UV,AUV1,AUV2,AUV3,AUV4,Material};
	typedef enum MorphMatType{MatMultiply=0,MatAdd=1};
};
class AMMorph;
//A group morph
struct MorphGroup
{
	AMMorph* GroupedMorph;
	float MorphRate;
};
//A vertex (normally facial) morph
struct MorphVertex
{
	MMDVERTEX* Vertex;
	D3DXVECTOR3 MaxOffset;
};
//A UV morph
struct MorphUV
{
	MMDVERTEX* Vertex;
	D3DXVECTOR2 MaxOffset;
};
//A bone morph
struct MorphBone
{
	AMBone* Bone;
	D3DXVECTOR3 MaxOffset;
	D3DXQUATERNION MaxRotation;
};
//A material morph
struct MorphMaterial
{
	PMD_Material* Material;
	MorphDefs::MorphMatType OffsetType;
	D3DXVECTOR4 Diffuse;
	D3DXVECTOR3 Specular;
	float Shininess;
	D3DXVECTOR3 Ambient;
	D3DXVECTOR4 EdgeColor;
	float EdgeSize;
	D3DXVECTOR4 TextureCoef;
	D3DXVECTOR4 SphereCoef;
	D3DXVECTOR4 ToonCoef;
};
//The morph info structure
struct MorphInfo
{
	char* Name;
	char* NameEnglish;
	MorphDefs::MorphCategory Category; //unused by AM
	MorphDefs::MorphType Type;
	long OffsetCount; //counts the number of "things" this morph affects; depends on type

	//the following will only be used if it is that type of morph
	MorphGroup* Groups;
	MorphVertex* Vertices;
	MorphUV* UVs;
	MorphBone* Bones;
	MorphMaterial* Materials;
};
/*
	Manages the calculation of morphs and the things they affect
*/
class AMMorph {
private:
	/*The morph info structure*/
	MorphInfo _info;
	/*The scalar set if this morph is affected by another group morph*/
	float _groupedScl;

public:
	AMMorph();
	~AMMorph();

	/*
		MorphInfo* getInfo()

		Get pointer to the morph info structure

		Returns:		Pointer to the morph info structure
	*/
	MorphInfo* getInfo();

	/*
		void calculate()

		Calculate anything affected by this morph based on a weight

		Parameters:		weight			Weight of effect (0.0->1.0)
	*/
	void calculate(float weight);
	/*
		void groupScl()

		Calculate the grouped scalar if this morph if affected by a group morph

		Parameters:		scl				Scalar value to use in calculation (0.0->1.0)
	*/
	void groupScl(float scl);
	/*
		void clearVertTransforms()

		Clear transforms for all vertices or UVs affected by this morph.
		Only works if this morph is of type Vertex or UV
	*/
	void clearVertTransforms();
};
#endif