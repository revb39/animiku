/************************************************************************
*	AM_Pmd.cpp -- File parsing for PMD format							*
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

#include "AM_PMD.h"

AM_Pmd::AM_Pmd()
{

}
AM_Pmd::~AM_Pmd()
{
	if (_nameJ)
		delete[] _nameJ;
	if (_nameE)
		delete[] _nameE;
	if (_commentJ)
		delete[] _commentJ;
	if (_commentE)
		delete[] _commentE;
	if (cVertices)
		delete[] cVertices;
	if (_surfaceList)
		delete[] _surfaceList;
	if (_material)
	{
		for (int i=0;i<(int)_numMaterial;i++)
		{
			if (_material[i].getInfo()->TextureFile)
				delete[] _material[i].getInfo()->TextureFile;
			if (_material[i].getInfo()->SphereFile)
				delete[] _material[i].getInfo()->SphereFile;
		}
		delete[] _material;
	}
	if (_morphList)
		delete[] _morphList;
	if (_constraintList)
		delete[] _constraintList;
	if (_rigidBodyList)
		delete[] _rigidBodyList;
}

bool AM_Pmd::parse(char* file, char* dir, bool ext)
{
	//get magic key and verify valid PMD file
	char magic[3];
	if (fread_s(magic, 3, 1, 3, f) < 3)
		return false;
	if (magic[0] != 'P' || magic[1] != 'm' || magic[2] != 'd')
		return false;
	//get the version number and verify correct version
	float v;
	if (fread_s(&v, 4, 1, 4, f) < 4)
		return false;
	if (v != 1.0f)
		return false;

	//get model name (in Japanese)
	_nameJ = new char[20];
	if (fread_s(_nameJ, 20, 1, 20, f) < 20)
		return false;

	//get model comment (in Japanese)
	_commentJ = new char[256];
	if (fread_s(_commentJ, 256, 1, 256, f) < 256)
		return false;

	//get all vertices in mesh
	if (fread_s(&_numVertex, 4, 1, 4, f) < 4)
		return false;
	cVertices = new MMDVERTEX[_numVertex];
	for (int i=0;i<(int)_numVertex;i++)
	{
		if (fread_s(&cVertices[i].pos, 12, 1, 12, f) < 12) //position
			return false;
		if (fread_s(&cVertices[i].normal, 12, 1, 12, f) < 12) //normal
			return false;
		if (fread_s(&cVertices[i].uv, 8, 1, 8, f) < 8) //uv
			return false;
		//PMD vertices only support skinning by 2 bones
		unsigned short bl1, bl2;
		if (fread_s(&bl1, 2, 1, 2, f) < 2) //skinning bone index 1
			return false;
		if (fread_s(&bl2, 2, 1, 2, f) < 2) //skinning bone index 2
			return false;
		cVertices[i].boneIndices = D3DXVECTOR4(bl1, bl2, -1, -1);

		unsigned char w;
		//get bone 0 weight (in percent)
		if (fread_s(&w, 1, 1, 1, f) < 1) //bone 0 weight
			return false;
		//bone 1 weight defined by 1-bone0Weight
		cVertices[i].boneWeights = D3DXVECTOR4((float)w*0.01f, 1.0f-((float)w*0.01f), 0, 0);

		unsigned char e;
		if (fread_s(&e, 1, 1, 1, f) < 1) //edge flag - ignore
			return false;

		//save original position and normal separately for this vertex
		cVertices[i].origPos = cVertices[i].pos;
		cVertices[i].origNormal = cVertices[i].normal;
		D3DXMatrixIdentity(&(cVertices[i].curFaceTrans));
	}

	//get all indices in mesh
	if (fread_s(&_numSurface, 4, 1, 4, f) < 4)
		return false;
	_surfaceList = new unsigned short[_numSurface];
	if (fread_s(_surfaceList, sizeof(unsigned short)*_numSurface, 1, sizeof(unsigned short)*_numSurface, f) < sizeof(unsigned short)*_numSurface)
		return false;

	//get all materials in model
	if (fread_s(&_numMaterial, 4, 1, 4, f) < 4)
		return false;
	_material = new PMD_Material[_numMaterial];
	MaterialInfo *mci = NULL;
	for (int i=0;i<(int)_numMaterial;i++)
	{
		mci = _material[i].getInfo();

		if (fread_s(&mci->Diffuse, 12, 1, 12, f) < 12) //diffuse
			return false;
		if (fread_s(&mci->Alpha, 4, 1, 4, f) < 4) //alpha
			return false;
		if (fread_s(&mci->Shininess, 4, 1, 4, f) < 4) //shininess
			return false;
		if (fread_s(&mci->Specular, 12, 1, 12, f) < 12) //specular
			return false;
		if (fread_s(&mci->Ambient, 12, 1, 12, f) < 12) //ambient
			return false;

		char toonedge[2];
		if (fread_s(&toonedge, 2, 1, 2, f) < 2) //toon # and edge flag - ignore both
			return false;

		unsigned long ac;
		if (fread_s(&ac, 4, 1, 4, f) < 4) //surface count
			return false;
		mci->ApexCount = (int)ac;

		char tex[20];
		if (fread_s(tex, 20, 1, 20, f) < 20) //texture & sphere data
			return false;
		
		//texture and sphere data written as "texture_file*sphere_file"
		char *id = strrchr(tex, '*');
		if (id) //there is a sphere map
		{
			//get the texture file name
			int l = id - &tex[0];
			mci->TextureFile = new char[l+1];
			strncpy_s(mci->TextureFile, l+1, tex, l);
			//get the sphere map file name
			mci->SphereFile = new char[strlen(id+1)+1];
			strcpy_s(mci->SphereFile, strlen(id+1)+1, id+1);
			//figure out the sphere mapping mode
			if (!strcmp(AMUtils::getExtension(mci->SphereFile), "sph"))
				mci->SphereMode = Multiply;
			else
				mci->SphereMode = Add;
		}
		else //there is no sphere map
		{
			//get the textire file name
			int l = strlen(tex);
			mci->TextureFile = new char[l+1];
			strcpy_s(mci->TextureFile, l+1, tex);

			//sphere mapping type is no sphere
			mci->SphereFile = NULL;
			mci->SphereMode = NoSphere;
		}

		//if there was a sphere map bu no texture, set texture file to null
		if (strlen(mci->TextureFile) <= 0)
				mci->TextureFile = NULL;
	}

	//get all bones in model
	unsigned short numBone;
	if (fread_s(&numBone, 2, 1, 2, f) < 2)
		return false;
	_numBone = numBone;
	AMBone* boneList = new AMBone[_numBone];
	BoneInfo* bci;
	for (int i=0;i<(int)_numBone;i++)
	{
		bci = boneList[i].getInfo();

		char n[20];
		if (fread_s(n, 20, 1, 20, f) < 20) //bone name
			return false;
		bci->Name = new char[20];
		memcpy_s(bci->Name, 20, n, 20);

		short bl;
		if (fread_s(&bl, 2, 1, 2, f) < 2) //parent bone ID
			return false;
		if (bl != -1)
			bci->Parent = &boneList[bl];
		if (fread_s(&bl, 2, 1, 2, f) < 2) //child bone ID
			return false;
		if (bl != -1)
			bci->FirstChild = &boneList[bl];

		char t;
		if (fread_s(&t, 1, 1, 1, f) < 1) //bone type
			return false;

		short target;
		if (fread_s(&target, 2, 1, 2, f) < 2)
			return false;

		//figure out the bone parameters based on the bone type
		switch ((BoneType)t)
		{
		case Rotate:
			bci->TransConfig.CanRotate = true;
			break;
		case RotateMove:
			bci->TransConfig.CanMove = true;
			bci->TransConfig.CanRotate = true;
			break;
		case IKType:
			bci->TransConfig.IK = true;
			bci->TransConfig.CanMove = true;
			bci->TransConfig.CanRotate = true;
			break;
		case None:
			//I have no idea why this type even exists, it literally has no purpose
			break;
		case IKFollow:
			bci->TransConfig.CanRotate = true;
			break;
		case InfRo:
			bci->TransConfig.RotationInherit = true;
			bci->TransVals.BoneInheritRotation = &boneList[target];
			bci->TransVals.RotationInhereRate = 1;
			break;
		case ArmX:
			bci->TransConfig.CanRotate = true;
			bci->TransConfig.FixedAxis = true;
			break;
		case ArmInv:
			bci->TransConfig.RotationInherit = true;
			bci->TransVals.BoneInheritRotation = &boneList[bl];
			bci->TransVals.RotationInhereRate = (float)target / 100.0f;
			break;
		}

		//get the initial world position of this bone
		if (fread_s(&bci->InitialPosition, 12, 1, 12, f) < 12)
			return false;

		//initialize the bone
		boneList[i].init();
	}
	//we're going to wait to calculate the rotation axis since the inherited bone can come after the "X" bone
	for (int i=0;i<(int)_numBone;i++)
	{
		bci = boneList[i].getInfo();

		if (bci->TransConfig.FixedAxis)
		{
			D3DXVECTOR3 axis;
			D3DXVECTOR3 ofs = bci->InitialPosition - bci->FirstChild->getInfo()->InitialPosition;
			D3DXVec3Normalize(&axis, &ofs);
			bci->TransVals.FixedAxis = axis;
		}
	}

	//initialize the entire bone structure
	boneController.setupBoneList(boneList, _numBone);

	//load all IK data
	unsigned short numIK;
	if (fread_s(&numIK, 2, 1, 2, f) < 2)
		return false;
	IK *ikList = new IK[numIK];
	IKInfo *ici;
	for (int i=0;i<(int)numIK;i++)
	{
		ici = ikList[i].getInfo();

		short bl;
		if (fread_s(&bl, 2, 1, 2, f) < 2) //destination bone
			return false;
		ici->DestinationBone = &boneList[bl];
		if (fread_s(&bl, 2, 1, 2, f) < 2) //end effector bone
			return false;
		ici->EndEffectorBone = &boneList[bl];

		unsigned char nl;
		if (fread_s(&nl, 1, 1, 1, f) < 1) //numer of links in ik chain
			return false;
		ici->NumberOfLinks = nl;
		ici->BoneLinkList = new IKLink[nl];

		unsigned short ic;
		if (fread_s(&ic, 2, 1, 2, f) < 2) //iteration count
			return false;
		ici->IterationCount = ic;

		if (fread_s(&ici->AngleConstraint, 4, 1, 4, f) < 4) //angle constraint
			return false;
		ici->AngleConstraint *= 4.0f;

		for (int l=0;l<(int)ici->NumberOfLinks;l++)
		{
			short bl;
			if (fread_s(&bl, 2, 1, 2, f) < 2) //link bone id
				return false;
			ici->BoneLinkList[l].LinkBone = &boneList[bl];

			if (strstr(boneList[bl].getInfo()->Name, PMDMODEL_KNEENAME)) //limit angle if knee bone
			{
				ici->BoneLinkList[l].AngleLimit = true;
				ici->BoneLinkList[l].LowerLimit = D3DXVECTOR3(-PI,0,0);
				ici->BoneLinkList[l].UpperLimit = D3DXVECTOR3(0.00872665f,0,0);
			}
			else
			{
				ici->BoneLinkList[l].AngleLimit = false;
				ici->BoneLinkList[l].LowerLimit = D3DXVECTOR3(0,0,0);
				ici->BoneLinkList[l].UpperLimit = D3DXVECTOR3(0,0,0);
			}
		}
	}

	//initialize the IK chain list in the bone controller
	boneController.setupIKList(ikList, numIK);

	//load all morph data
	unsigned short nm;
	if (fread_s(&nm, 2, 1, 2, f) < 2)
		return false;
	_numMorph = nm;
	_morphList = new AMMorph[nm];
	MorphInfo *fci;
	for (int i=0;i<(int)nm;i++)
	{
		fci = _morphList[i].getInfo();

		fci->Name = new char[20];
		if (fread_s(fci->Name, 20, 1, 20, f) < 20) //name
			return false;

		fci->Type = MorphDefs::Vertex; //PMD only works with vertex morphs

		if (fread_s(&fci->OffsetCount, 4, 1, 4, f) < 4) //offset count
			return false;
		fci->Vertices = new MorphVertex[fci->OffsetCount];

		unsigned char grp;
		if (fread_s(&grp, 1, 1, 1, f) < 1) //display group - not used
			return false;

		//loop through and read each offset
		for (int v=0;v<fci->OffsetCount;v++)
		{	
			unsigned long ind;
			if (fread_s(&ind, 4, 1, 4, f) < 4) //vertex index
				return false;
			if (strcmp(fci->Name, "base"))
				fci->Vertices[v].Vertex = _morphList[0].getInfo()->Vertices[ind].Vertex; //morph vertex is actually an index into the base face vertex list
			else
				fci->Vertices[v].Vertex = &cVertices[ind];
			
			if(fread_s(&fci->Vertices[v].MaxOffset, 12, 1, 12, f) < 12) //max offset
				return false;
		}
	}

	//display data - only used by MMD for displaying and ordering things in the UI
	unsigned char n, nb;
	unsigned long nbd;
	if (fread_s(&n, 1, 1, 1, f) < 1)
		return false;
	int sz = sizeof(unsigned short)*n;
	char *buf = new char[sz];
	if (fread_s(buf, sz, 1, sz, f) < (size_t)sz)
		return false;
	delete[] buf;

	if (fread_s(&nb, 1, 1, 1, f) < 1)
		return false;
	sz = 50 * nb;
	buf = new char[sz];
	if (fread_s(buf, sz, 1, sz, f) < (size_t)sz)
		return false;
	delete[] buf;

	if (fread_s(&nbd, 4, 1, 4, f) < 4)
		return false;
	sz = (sizeof(short)+sizeof(unsigned char))*nbd;
	buf = new char[sz];
	if (fread_s(buf, sz, 1, sz, f) < (size_t)sz)
		return false;
	delete[] buf;

	if (feof(f)) //if no additional sections, parsing complete
		return true;

	//english model information
	unsigned char e = 0;
	if (fread_s(&e, 1, 1, 1, f) < 1) //flag telling if english model information exists
		return false;
	if (e != 0)
	{
		_nameE = new char[20];
		if (fread_s(_nameE, 20, 1, 20, f) < 20) //name
			return false;
		_commentE = new char[256];
		if (fread_s(_commentE, 256, 1, 256, f) < 256) //comment
			return false;

		for (int i=0;i<(int)_numBone;i++) //bone names
		{
			bci = boneList[i].getInfo();

			bci->NameEnglish = new char[20];
			if (fread_s(bci->NameEnglish, 20, 1, 20, f) < 20)
				return false;
		}
		for (int i=1;i<(int)_numMorph;i++) //morph names (skip base morph)
		{
			fci = _morphList[i].getInfo();

			fci->NameEnglish = new char[20];
			if (fread_s(fci->NameEnglish, 20, 1, 20, f) < 20)
				return false;
		}
		for (int i=0;i<(int)nb;i++) //bone group name - skip all, not used
		{
			char *name = new char[50];
			if (fread_s(name, 50, 1, 50, f) < 50)
				return false;
			delete[] name;
		}
	}

	if (feof(f)) //if no additional sections, parsing complete
		return true;

	//toon textures - not supported by AMENGINE
	for (int i=1;i<=10;i++)
	{
		char* newToonName = new char[100];
		if (fread_s(newToonName, 100, 1, 100, f) < 100)
			return false;
		delete[] newToonName;
	}

	if (feof(f)) //if no additional sections, parsing complete
		return true;

	//load all rigid bodies
	if (fread_s(&_numRigidBody, 4, 1, 4, f) < 4)
		return false;
	_rigidBodyList = new AMRigidBody[_numRigidBody];
	RigidBodyInfo *rci;
	for (int i=0;i<(int)_numRigidBody;i++)
	{
		rci = _rigidBodyList[i].getInfo();

		rci->Name = new char[20];
		if (fread_s(rci->Name, 20, 1, 20, f) < 20) //name
			return false;

		unsigned short bl;
		if (fread_s(&bl, 2, 1, 2, f) < 2) //bone the body is attached to
			return false;
		if (bl == 0xFFFF)
			rci->RelatedBone = boneController.getCenterBone();
		else
			rci->RelatedBone = &boneList[bl];

		if (fread_s(&rci->Group, 1, 1, 1, f) < 1) //collision group
			return false;
		if (fread_s(&rci->NonCollisionMask, 2, 1, 2, f) < 2) //non-collision mask
			return false;

		unsigned char shape;
		if (fread_s(&shape, 1, 1, 1, f) < 1) //body shape
			return false;
		rci->Shape = (RigidBodyDefs::RigidBodyShape)shape;

		if (fread_s(&rci->BodySize, 12, 1, 12, f) < 12) //body size
			return false;

		if (fread_s(&rci->RelativePos, 12, 1, 12, f) < 12) //position relative to related bone
			return false;

		if (fread_s(&rci->RelativeRot, 12, 1, 12, f) < 12) //rotation relative to related bone
			return false;

		if (fread_s(&rci->PhysConstants.Mass, 4, 1, 4, f) < 4) //mass
			return false;
		if (fread_s(&rci->PhysConstants.LinearDamping, 4, 1, 4, f) < 4) //linear damping
			return false;
		if (fread_s(&rci->PhysConstants.AngularDamping, 4, 1, 4, f) < 4) //angular damping
			return false;
		if (fread_s(&rci->PhysConstants.Restitution, 4, 1, 4, f) < 4) //restitution
			return false;
		if (fread_s(&rci->PhysConstants.Friction, 4, 1, 4, f) < 4) //friction
			return false;

		unsigned char type;
		if (fread_s(&type, 1, 1, 1, f) < 1) //body type
			return false;
		rci->Type = (RigidBodyDefs::RigidBodyType)type;

		//initialize the rigid body
		_rigidBodyList[i].init(_pPhysics->getWorld());
	}

	if (feof(f)) //if no additional sections, parsing complete
		return true;

	//load all physics constraints
	if (fread_s(&_numConstraint, 4, 1, 4, f) < 4)
		return false;
	_constraintList = new AMConstraint[_numConstraint];
	ConstraintInfo* cci;
	for (int i=0;i<(int)_numConstraint;i++)
	{
		cci = _constraintList[i].getInfo();

		cci->Name = new char[20];
		if (fread_s(cci->Name, 20, 1, 20, f) < 20) //constraint name
			return false;

		unsigned long body;
		if (fread_s(&body, 4, 1, 4, f) < 4) //body A
			return false;
		if (body >= 0)
			cci->BodyA = &_rigidBodyList[body];
		if (fread_s(&body, 4, 1, 4, f) < 4) //body B
			return false;
		if (body >= 0)
			cci->BodyB = &_rigidBodyList[body];

		if (fread_s(cci->InitialPos, 12, 1, 12, f) < 12) //relative position
			return false;
		if (fread_s(cci->InitialRot, 12, 1, 12, f) < 12) //relative rotation
			return false;

		if (fread_s(cci->PosLimitLower, 12, 1, 12, f) < 12) //pos lower limit
			return false;
		if (fread_s(cci->PosLimitUpper, 12, 1, 12, f) < 12) //pos upper limit
			return false;
		if (fread_s(cci->RotLimitLower, 12, 1, 12, f) < 12) //rot lower limit
			return false;
		if (fread_s(cci->RotLimitUpper, 12, 1, 12, f) < 12) //rot upper limit
			return false;

		if (fread_s(cci->Stiffness, 24, 1, 24, f) < 24) //stiffness
			return false;

		_constraintList[i].init(_pPhysics->getWorld());
	}

	//parse the extended data if it exists
	if (ext)
		extendedData.loadAndParse(f, _numMaterial, dev);

	//prepare everything for D3D
	prepareD3D();
	return true;
}