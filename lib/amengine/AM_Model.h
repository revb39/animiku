/************************************************************************
*	AM_Model.h -- Model management										*
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

#ifndef MODEL_H_
#define MODEL_H_

#include "AM_BoneController.h"
#include "AM_Morph.h"
#include "AM_Fader.h"
#include "AM_Extended.h"

class AM_Model;
/*Stuff needed for dynamic types*/
struct ModelType
{
	char* Description;
	void* Constructor;
};
/*Just call RegisterModelType to link a file extension to a class constructor*/
#define RegisterModelType(i,d,t) (AM_Model::ModelParsers()[i].Description = d);(AM_Model::ModelParsers()[i].Constructor = &createInstanceModel<t>)
template<class T> AM_Model* createInstanceModel() {return new T;}
typedef map<string, ModelType> InhModelParsers;

class AMSettings;

/*
	Handles all aspects of preparing and displaying models
*/
class AM_Model {
protected:
	//
	// Stuff for D3D rendering
	//
	/*Pointer to the D3D device*/
	ID3D10Device *dev;

	/*Vertex and index buffer for mesh rendering*/
	ID3D10Buffer *v_buffer;
	ID3D10Buffer *i_buffer;

	/*Resources for texture rendering*/
	ID3D10Resource *textures, *spheres;
	D3D10_SHADER_RESOURCE_VIEW_DESC texDesc, sphereDesc;
	ID3D10ShaderResourceView **texResViews, **sphereResViews;

	/*World transform matrix*/
	ID3D10EffectMatrixVariable *world;
	/*Bone matrix palette for vertex blending*/
	ID3D10EffectMatrixVariable *palette;
	/*Material ambient color*/
	ID3D10EffectVectorVariable *amb;
	/*Material diffuse color*/
	ID3D10EffectVectorVariable *dif;
	/*Material specular color*/
	ID3D10EffectVectorVariable *spec;
	/*Material shininess*/
	ID3D10EffectScalarVariable *shine;
	/*Material alpha*/
	ID3D10EffectScalarVariable *alpha;

	/*Rendering flags*/
	ID3D10EffectScalarVariable *useTex;
	ID3D10EffectScalarVariable *useToon;
	ID3D10EffectScalarVariable *sphType;
	ID3D10EffectScalarVariable *lightsOff;
	ID3D10EffectScalarVariable *extraBright;
	ID3D10EffectScalarVariable *fadeCoef;

	/*Texture resources*/
	ID3D10EffectShaderResourceVariable *tex;
	ID3D10EffectShaderResourceVariable *sph;
	ID3D10EffectShaderResourceVariable *toon;

	/*Pointers to the fx and rendering technique*/
	ID3D10Effect *effect;
	ID3D10EffectTechnique *effectTech;

	/*The actual bone batrix palette for vertex blending*/
	D3DXMATRIX *boneMatrixPalette2;

	//
	// raw data from parsing
	//
	/*Name in Japanese*/
	char* _nameJ;
	/*Name in English*/
	char* _nameE;
	/*Comment in Japanese*/
	char* _commentJ;
	/*Comment in English*/
	char* _commentE;

	/*Number of vertices in mesh*/
	unsigned long _numVertex;
	/*List of vertices in the mesh*/
	MMDVERTEX *cVertices;
	/*Number of surfaces (indices) in the mesh*/
	unsigned long _numSurface;
	/*List of surfaces (indices) in the mesh*/
	unsigned short *_surfaceList;

	/*Number of bones in the model*/
	unsigned long _numBone;

	/*Number of materials in the model*/
	unsigned long _numMaterial;
	/*List of material data blocks*/
	PMD_Material *_material;

	/*Number of facial morphs*/
	unsigned long _numMorph;
	/*List of facial morphs*/
	AMMorph *_morphList;
	/*Pointer to the base morph*/
	AMMorph *_baseMorph;
	/*Total number of facial morph affected vertices in model*/
	long numFaceVertex;
	/*List of all facial morph affected vertices in model*/
	int *faceVertexIndices;

	/*Number of rigid bodies*/
	unsigned long _numRigidBody;
	/*List of rigid bodies*/
	AMRigidBody *_rigidBodyList;
	/*Pointer to the Bullet Physics engine object*/
	BulletPhysics *_pPhysics;

	/*Number of rigid body constraints*/
	unsigned long _numConstraint;
	/*List of rigid body constraints*/
	AMConstraint *_constraintList;

	/*The bone controller for this model*/
	AMBoneController boneController;
	/*The fader for this model*/
	AMFader fader;
	/*The extended data parser for this model*/
	AMExtended extendedData;

	/*World translation matrix of model*/
	D3DXMATRIX matTranslate;
	/*X rotation of model*/
	D3DXMATRIX matRotateX;
	/*Y rotation of model*/
	D3DXMATRIX matRotateY;
	/*Z rotation of model*/
	D3DXMATRIX matRotateZ;

	/*Will fade in ecchi mode?*/
	bool alphaFade;

	/*Raw model file data*/
	unsigned char* data;
	/*File handler*/
	FILE *f;
	/*Size of the model file*/
	size_t size;

	/*
		virtual bool parse()

		Virtual function implemented by subclasses to parse different file types

		Parameters:			file			Path to file to parse
							dir				Directory of file to parse
							ext				true/false: the model has AM extended data

		Returns:			bool			Parsing completed sucessfully
	*/
	virtual bool parse(char *file, char *dir, bool ext);
	/*
		void prepareD3D()

		Initialize data structures for D3D
	*/
	void prepareD3D();
	/*
		void updateFaceVertices()

		Calculate morphed vertex positions in model

		Parameters:			isDefPos		Update using the bind pose morph weights
	*/
	void updateMorphVertices(bool isDefPos);
	/*
		void updateFromPhysics()

		Update model using current physics rigid body transforms
	*/
	void updateFromPhysics();

public:
	AM_Model();
	virtual ~AM_Model();

	/*The motion assigned to this model*/
	AM_Motion *assignedMotion;

	/*
		bool load()

		Load and parse a model file

		Parameters:			file			Path to file to load
							mainDev			Pointer to D3D device
							phy				Pointer to Bullet Physics
		Returns:			bool			Model was loaded & parsed successfully
	*/
	bool load(char *file, ID3D10Device *mainDev, BulletPhysics* phy);

	/*
		void render()

		Render the model with D3D

		Parameters:			sets			Pointer to the settings object
							t				Pointer to D3D technique used to render model
	*/
	void render(AMSettings *sets, ID3D10EffectTechnique *t);
	/*
		void updateDevice()

		Create buffers for vertices, indices and texture data

		Parameters:			dev				Pointer to the D3D device
							effect			Pointer to the effect (shader)
							t				Pointer to the technique used to render model
	*/
	void updateDevice(ID3D10Device *dev, ID3D10Effect *effect, ID3D10EffectTechnique *t);

	/*
		void translate()

		Set world translation of model

		Parameters:			x,y,z			X,Y,Z translation of model
	*/
	void translate(float x, float y, float z);
	/*
		void rotate()

		Set euler rotation of model

		Parameters:			x,y,z			X,Y,Z rotation in radians of model
	*/
	void rotate (float x, float y, float z);
	/*
		AMFader* getFader()

		Get the model's fader

		Returns:			Pointer to the model's fader
	*/
	AMFader* getFader();
	/*
		void setAlphaFade()

		Set to use alpha (ecchi) fade

		Parameters:			t				true/false: Use alpha fade
	*/
	void setAlphaFade(bool t);

	/*
		void assignMotion()

		Assign a motion to this model

		Parameters:			motion			Pointer to the motion to assign
	*/
	void assignMotion(AM_Motion *motion);
	/*
		void startMotion()

		Start playback of the assigned motion at beginning
	*/
	void startMotion();
	/*
		void stopMotion()

		Ends playback of the assigned motion. Does not reset to beginning
	*/
	void stopMotion();
	/*
		bool hasMotionStarted()

		Gets if the assigned motion is playing or there is no assigned motion

		Returns:			true/false: The assigned motion is playing
	*/
	bool hasMotionStarted(); //return true if the motion should be currently playing
	/*
		void updateMotion()

		Update assigned motion's frame counter and tell bone controller to update
		bone structure
	*/
	void updateMotion();
	/*
		void moveToDefMotionPos()

		Move the model to the assigned motion's bind pose
	*/
	void moveToDefMotionPos();
	/*
		void resetPhysics()

		Force all rigid bodies to move to their bones' positions
	*/
	void resetPhysics();
	/*
		void setPhysicsEnabled()

		Set the physics simulation enabled/disabled

		Parameters:			e				true=enabled, false=disabled
	*/
	void setPhysicsEnabled(bool e);

	static InhModelParsers &ModelParsers()
	{
		static InhModelParsers mp;
		return mp;
	}
	static AM_Model* CreateFromType(char* type);
};

#endif