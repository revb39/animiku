/************************************************************************
*	AM_PMX.cpp -- File parsing for PMX format							*
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

#include "AM_PMX.h"
#include <ios>

AM_Pmx::AM_Pmx()
{
	_enc = UTF16;
	_uvCount = 0;
	_indexSize = 0;
	_texIdSize = 0;
	_matIdSize = 0;
	_boneIdSize = 0;
	_faceIdSize = 0;
	_rigidBodyIdSize = 0;

	_nameJ = NULL;
	_nameE = NULL;
	_commentJ = NULL;
	_commentE = NULL;

	_numTextures = 0;
	_textureList = NULL;
}
AM_Pmx::~AM_Pmx()
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
		delete[] _material;
	if (_morphList)
		delete[] _morphList;
	if (_constraintList)
		delete[] _constraintList;
	if (_rigidBodyList)
		delete[] _rigidBodyList;
	if (_textureList)
	{
		for (int i=0;i<(int)_numTextures;i++)
		{
			if (_textureList[i])
				delete[] _textureList[i];
		}
		delete[] _textureList;
	}
}

void AM_Pmx::getString(char** str)
{
	unsigned long sz;
	if (fread_s(&sz, sizeof(unsigned long), 1, 4, f) < 4)
	{
		*str = NULL;
		return;
	}

	//convert to ANSI
	switch (_enc)
	{
	case UTF16:
		{
			int q = sizeof(wchar_t);
			wchar_t *win= new WCHAR[(sz/2)+1]; //a wchar_t is actually 2 bytes
			if (fread_s(win, sz, 1, sz, f) < sz)
			{
				delete[] win;
				*str = NULL;
				return;
			}
			win[sz/2] = '\0';
			*str = UTF16ToANSI(win);
			delete[] win;
			break;
		}
	case UTF8:
		{
			char *cin = new char[sz+1];
			if (fread_s(cin, sz, 1, sz, f) < sz)
			{
				delete[] cin;
				*str = NULL;
				return;
			}
			cin[sz] = '\0';
			*str = UTF8ToANSI(cin);
		}
		break;
	}
}
char* AM_Pmx::UTF16ToANSI(WCHAR *win)
{
	char* ansi = NULL;
	int n = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, win, -1, NULL, 0, 0, 0);
	if (n > 1)
	{
		ansi = new char[n];
		WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, win, -1, ansi, n, 0, 0);
		return ansi;
	}

	return NULL;
}
char* AM_Pmx::UTF8ToANSI(char *cin)
{
	//must convert to UTF16 first
	char* ansi = NULL;
	int n = MultiByteToWideChar(CP_UTF8, 0, cin, -1, NULL, 0);
	if (n > 0)
	{
		wchar_t *wcs = new wchar_t[n];
		MultiByteToWideChar(CP_UTF8, 0, cin, -1, wcs, n);

		ansi = UTF16ToANSI(wcs);
	}
	return ansi;
}

void AM_Pmx::getVertexBoneLinks(int v)
{
	switch (cVertices[v].weightType)
	{
	case BDEF1:
		{
		//1 bone, weight of 1
		cVertices[v].boneIndices[0] = (float)indexConverter(_boneIdSize);
		cVertices[v].boneIndices[1] = -1;
		cVertices[v].boneIndices[2] = -1;
		cVertices[v].boneIndices[3] = -1;

		cVertices[v].boneWeights[0] = 1;
		cVertices[v].boneWeights[1] = 0;
		cVertices[v].boneWeights[2] = 0;
		cVertices[v].boneWeights[3] = 0;
		}
		break;
	case BDEF2:
		{
		//2 bones, weight from file
		cVertices[v].boneIndices[0] = (float)indexConverter(_boneIdSize);
		cVertices[v].boneIndices[1] = (float)indexConverter(_boneIdSize);
		cVertices[v].boneIndices[2] = -1;
		cVertices[v].boneIndices[3] = -1;

		float w;
		if (fread_s(&w, sizeof(float), 1, 4, f) < 4)
			w = 0;
		cVertices[v].boneWeights[0] = w;
		cVertices[v].boneWeights[1] = 1.0f-w;
		cVertices[v].boneWeights[2] = 0;
		cVertices[v].boneWeights[3] = 0;
		}
		break;
	case BDEF4:
		//4 bones, weights for each from file
		cVertices[v].boneIndices[0] = (float)indexConverter(_boneIdSize);
		cVertices[v].boneIndices[1] = (float)indexConverter(_boneIdSize);
		cVertices[v].boneIndices[2] = (float)indexConverter(_boneIdSize);
		cVertices[v].boneIndices[3] = (float)indexConverter(_boneIdSize);

		for (int i=0;i<4;i++)
		{
			float w;
			if (fread_s(&w, sizeof(float), 1, 4, f) < 4)
				w = 0;

			cVertices[v].boneWeights[i] = w;
		}
		break;
	case SDEF:
		{
		//nope, not a clue
		cVertices[v].boneIndices[0] = (float)indexConverter(_boneIdSize);
		cVertices[v].boneIndices[1] = (float)indexConverter(_boneIdSize);
		cVertices[v].boneIndices[2] = -1;
		cVertices[v].boneIndices[3] = -1;

		float w;
		if (fread_s(&w, sizeof(float), 1, 4, f) < 4)
			w = 0;
		cVertices[v].boneWeights[0] = w;
		cVertices[v].boneWeights[1] = 0;
		cVertices[v].boneWeights[2] = 0;
		cVertices[v].boneWeights[3] = 0;

		fread_s(&cVertices[v].SDSF_C, sizeof(D3DXVECTOR3), 1, 12, f);
		fread_s(&cVertices[v].SDSF_R0, sizeof(D3DXVECTOR3), 1, 12, f);
		fread_s(&cVertices[v].SDSF_R1, sizeof(D3DXVECTOR3), 1, 12, f);
		break;
		}
	}
}

int AM_Pmx::indexConverter(unsigned int nb)
{
	switch (nb)
	{
	case 1:
		{
		char n;
		if (fread_s(&n, nb, 1, nb, f) < nb)
			return -1;
		if (n < -1)
			return (int)((unsigned char)n); //guess to see if this is supposed to be unsigned or not
		return (int)n;
		break;
		}
	case 2:
		{
		short n;
		if (fread_s(&n, nb, 1, nb, f) < nb)
			return -1;
		if (n < -1)
			return (int)((unsigned short)n); //guess to see if this is supposed to be unsigned or not
		return (int)n;
		}
		break;
	case 4:
		{
		long n;
		if (fread_s(&n, nb, 1, nb, f) < nb)
			return -1;
		if (n < -1)
			return (int)((unsigned long)n); //guess to see if this is supposed to be unsigned or not
		return (int)n;
		}
		break;
	default:
		return -1;
	}
}

bool AM_Pmx::parse(char *file, char *dir, bool ext)
{
	//get the PMX identifier to make sure this is actually a pmx file
	char id[5];
	if (fread_s(id, sizeof(id), 1, 4, f) < 4)
		return false;
	id[4] = '\0';
	if (strcmp(id, "PMX "))
		return false;

	//get the version number
	float v;
	if (fread_s(&v, sizeof(float), 1, 4, f) < 4)
		return false;
	if (v != 2.0f)
		return false;


	//get the sizes array
	unsigned char sz;
	unsigned char *sa;
	//first the size of the array
	if (fread_s(&sz, sizeof(char), 1, 1, f) < 1)
		return false;
	//then read the array
	sa = new unsigned char[sz];
	if (fread_s(sa, sz, 1, sz, f) < sz)
		return false;
	//and get the values
	_enc = (Encoding)sa[0];
	_uvCount = (int)sa[1];
	_indexSize = (int)sa[2];
	_texIdSize = (int)sa[3];
	_matIdSize = (int)sa[4];
	_boneIdSize = (int)sa[5];
	_faceIdSize = (int)sa[6];
	_rigidBodyIdSize = (int)sa[7];

	delete[] sa; //not needed anymore b/c data has been copied to private vars
	
	//name and comment
	getString(&_nameJ);
	getString(&_nameE);
	getString(&_commentJ);
	getString(&_commentE);

	//vertices
	if (fread_s(&_numVertex, sizeof(unsigned long), 1, 4, f) < 4)
		return false;
	cVertices = new MMDVERTEX[_numVertex];
	for (int i=0;i<(int)_numVertex;i++)
	{
		if (fread_s(&cVertices[i].pos, sizeof(D3DXVECTOR3), 1, 12, f) < 12)
			return false;
		if (fread_s(&cVertices[i].normal, sizeof(D3DXVECTOR3), 1, 12, f) < 4*3)
			return false;
		if (fread_s(&cVertices[i].uv, sizeof(D3DXVECTOR2), 1, 8, f) < 4*2)
			return false;

		cVertices[i].origPos = cVertices[i].pos;
		cVertices[i].origNormal = cVertices[i].normal;
		cVertices[i].origUV = cVertices[i].uv;
		D3DXMatrixIdentity(&cVertices[i].curFaceTrans);
		D3DXMatrixIdentity(&cVertices[i].curFaceTransUV);

		//what the shit is this
		if (_uvCount > 0)
		{
			for (int u=0;u<_uvCount;u++)
			{
				if (fread_s(&cVertices[i].addingUV[u], sizeof(D3DXVECTOR4), 1, 16, f) < 16)
					return false;
			}
		}

		//weight type
		char t;
		if (fread_s(&t, sizeof(char), 1, 1, f) < 1)
			return false;
		cVertices[i].weightType = (WeightType)((int)t);

		getVertexBoneLinks(i);

		unsigned long edgeRate; //unused
		fread_s(&edgeRate, sizeof(unsigned long), 1, 4, f);
	}

	//indices
	if (fread_s(&_numSurface, sizeof(unsigned long), 1, 4, f) < 4)
		return false;
	_surfaceList = new unsigned short[_numSurface];
	for (int i=0;i<(int)_numSurface;i++)
	{
		_surfaceList[i] = indexConverter(_indexSize);
	}

	//textures
	if (fread_s(&_numTextures, sizeof(unsigned long), 1, 4, f) < 4)
		return false;
	_textureList = new char*[_numTextures];
	for (int i=0;i<(int)_numTextures;i++)
	{
		getString(&_textureList[i]);
	}

	//materials
	if (fread_s(&_numMaterial, sizeof (unsigned long), 1, 4, f) < 4)
		return false;

	_material = new PMD_Material[_numMaterial];

	char* tmp;
	float tmpFA[3];
	float tmpF;
	MaterialInfo *ci;
	for (int i = 0; i < (int)_numMaterial; i++)
	{
		ci = _material[i].getInfo();

		getString(&tmp); //name; ignore
		delete[] tmp;
		getString(&tmp); //english name; ignore
		delete[] tmp;

		//diffuse
		if (fread_s(&tmpFA, sizeof(float) * 3, 1, 12, f) < 12)
			return false;
		ci->Diffuse = Color3::FromArray(tmpFA);
		//diffuse alpha
		if (fread_s(&tmpF, sizeof(float), 1, 4, f) < 4)
			return false;
		ci->Alpha = tmpF;
		//specular
		if (fread_s(&tmpFA, sizeof(float) * 3, 1, 12, f) < 12)
			return false;
		ci->Specular = Color3::FromArray(tmpFA);
		//specular power
		if (fread_s(&tmpF, sizeof(float), 1, 4, f) < 4)
			return false;
		ci->Shininess = tmpF;
		//ambient
		if (fread_s(&tmpFA, sizeof(float) * 3, 1, 12, f) < 12)
			return false;
		ci->Ambient = Color3::FromArray(tmpFA);

		char flags; //material flags; ignore for now
		if (fread_s(&flags, sizeof(char), 1, 1, f) < 1)
			return false;

		//edge color and size; ignore
		char edge[20];
		if (fread_s(edge, sizeof(char) * 20, 1, 20, f) < 20)
			return false;

		//texture files
		int texId = indexConverter(_texIdSize);
		int sphereId = indexConverter(_texIdSize);

		//mapping info
		char map[2];
		if (fread_s(map, sizeof(map), 1, 2, f) < 2)
			return false;
		ci->SphereMode = (SphereMode)map[0]; //set the sphere mapping technique
		//toon texture type
		//regardless, toon shading not supported in AMENGINE so throw data away
		if (map[1] == 1)
		{
			char id;
			if (fread_s(&id, sizeof(char), 1, 1, f) < 1)
				return false;
		}
		else
		{
			indexConverter(_texIdSize);
		}

		char* memo;
		getString(&memo); //ignore this
		delete[] memo;

		//surface count
		unsigned long si;
		if (fread_s(&si, sizeof(unsigned long), 1, 4, f) < 4)
			return false;
		ci->ApexCount = si;

		//do the texture/sphere map crap
		int sz = 1;
		if (texId == -1)
			ci->TextureFile = NULL;
		else
			ci->TextureFile = _textureList[texId];

		if (sphereId == -1)
			ci->SphereFile = NULL;
		else
			ci->SphereFile = _textureList[sphereId];
	}
	//bones
	unsigned long nb = 0;
	if (fread_s(&nb, sizeof(unsigned long), 1, 4, f) < 4)
		return false;
	_numBone = nb;

	AMBone* bl = new AMBone[nb];
	IKInfo* ici;
	vector<IK*> ikList;
	BoneInfo* bci;

	for (int i=0;i<(int)nb;i++)
	{
		bci = bl[i].getInfo();

		//bone name
		getString(&bci->Name);

		//bone english name
		getString(&bci->NameEnglish);

		if (fread_s(&bci->InitialPosition, 12, 1, 12, f) < 12) //get the bone's initial position
			return false;

		int parentIndex = indexConverter(_boneIdSize);
		if (parentIndex == -1 || parentIndex >= (int)_numBone)
			bci->Parent = NULL;
		else
			bci->Parent = &bl[parentIndex];

		unsigned long transformRank;
		if (fread_s(&transformRank, sizeof(unsigned long), 1, 4, f) < 4) //this can be ignored for now, I have no idea what this does
			return false;

		//flags
		unsigned short fl;
		if (fread_s(&fl, sizeof(unsigned short), 1, 2, f) < 2)
			return false;

		bool connectWithChild = ((fl & 0x0001) > 0);
		if (!connectWithChild)
		{
			char buf[12];
			//since we don't use the offset, just read it to skip it
			if (fread_s(buf, 12, 1, 12, f) < 12)
				return false;

			bci->FirstChild = NULL;
		}
		else
		{
			//we will use the child to help in the traversing algorithm
			int child = indexConverter(_boneIdSize);
			if (child == -1 || child >= (int)_numBone)
				bci->FirstChild = NULL;
			else
				bci->FirstChild = &bl[child];
		}
		bci->TransConfig.CanRotate = ((fl & 0x0002) > 0);
		bci->TransConfig.CanMove = ((fl & 0x0004) > 0);
		//ignore display and operation flags, we don't need them
		bci->TransConfig.IK = ((fl & 0x0020) > 0);
		bci->TransConfig.RotationInherit = ((fl & 0x100) > 0);
		if (bci->TransConfig.RotationInherit)
		{
			int rotBoneIndex = indexConverter(_boneIdSize);
			bci->TransVals.BoneInheritRotation = &bl[rotBoneIndex];
			if (fread_s(&bci->TransVals.RotationInhereRate, 4, 1, 4, f) < 4)
				return false;
		}
		bci->TransConfig.TranslationInherit = ((fl & 0x0200) > 0);
		if (bci->TransConfig.TranslationInherit)
		{
			bci->TransVals.BoneInheritTranslation = bci->TransVals.BoneInheritRotation;
			bci->TransVals.TranslationInhereRate = bci->TransVals.RotationInhereRate;
			/*int transBoneIndex = indexConverter(_boneIdSize);
			bci->TransVals.BoneInheritTranslation = &bl[transBoneIndex];
			if (fread_s(&bci->TransVals.TranslationInhereRate, 4, 1, 4, f) < 4)
				return false;*/
		}
		bci->TransConfig.FixedAxis = ((fl & 0x0400) > 0);
		if (bci->TransConfig.FixedAxis)
		{
			//the rotation axis is locked to this direction vector
			if (fread_s(&bci->TransVals.FixedAxis, 12, 1, 12, f) < 12)
				return false;
		}
		bci->TransConfig.LocalAxes = ((fl & 0x0800) > 0);
		if (bci->TransConfig.LocalAxes)
		{
			//the rotation axis is locked to this local axis (?)
			if (fread_s(&bci->TransVals.LocalAxisX, 12, 1, 12, f) < 12)
				return false;
			if (fread_s(&bci->TransVals.LocalAxisZ, 12, 1, 12, f) < 12)
				return false;
		}
		bci->TransConfig.AfterPhysicalDeformation = ((fl & 0x1000) > 0);
		bci->TransConfig.ExternalParentDeformation = ((fl & 0x2000) > 0);
		if (bci->TransConfig.ExternalParentDeformation)
		{
			if (fread_s(&bci->TransVals.ExternalParentDeformationKey, 4, 1, 4, f) < 4)
				return false;
		}

		if (bci->TransConfig.IK)
		{
			IK *newIk = new IK();
			ici = newIk->getInfo();
			ici->DestinationBone = &bl[i];
			int endEffectorIndex = indexConverter(_boneIdSize); //get the target bone
			ici->EndEffectorBone = &bl[endEffectorIndex];
			if (fread_s(&ici->IterationCount, 4, 1, 4, f) < 4) //get the iteration count
				return false;
			if (fread_s(&ici->AngleConstraint, 4, 1, 4, f) < 4) //get the angle constraint
				return false;
			if (fread_s(&ici->NumberOfLinks, 4, 1, 4, f) < 4) //get number of links in IK chain
				return false;
			ici->BoneLinkList = new IKLink[ici->NumberOfLinks]; //allocate memory for ik link list
			for (int l=0;l<(int)ici->NumberOfLinks;l++) //build the ik chain link list
			{
				int linkIndex = indexConverter(_boneIdSize);
				ici->BoneLinkList[l].LinkBone = &bl[linkIndex]; //get the link bone
				unsigned char angleLimit = 0;
				if (fread_s(&angleLimit, 1, 1, 1, f) < 1) //get whether this link is angle limited or now
					return false;
				ici->BoneLinkList[l].AngleLimit = (angleLimit == 1);
				if (angleLimit == 1)
				{
					if (fread_s(&ici->BoneLinkList[l].LowerLimit, 12, 1, 12, f) < 12) //get the lower angle limit
						return false;
					if (fread_s(&ici->BoneLinkList[l].UpperLimit, 12, 1, 12, f) < 12) //get the upper angle limit
						return false;
				}
			}
			ikList.push_back(newIk);
		}

		bl[i].init();
	}
	boneController.setupBoneList(bl, _numBone);
	int nik = ikList.size(); //store the number of iks since we will be freeing mem in the vector
	ikl = new IK[nik];
	for (int i=0;i<(int)ikList.size();i++)
	{
		ikl[i] = *ikList[i];
		delete ikList[i];
	}
	boneController.setupIKList(ikl, nik);

	//morphs
	unsigned long nm;
	if (fread_s(&nm, sizeof(unsigned long), 1, 4, f) < 4)
		return false;
	_numMorph = nm;
	_morphList = new AMMorph[nm];

	MorphInfo *mci;
	for (int i=0;i<(int)nm;i++)
	{
		mci = _morphList[i].getInfo();

		getString(&mci->Name);
		getString(&mci->NameEnglish);

		unsigned char cat;
		if (fread_s(&cat, sizeof(unsigned char), 1, 1, f) < 1) //get the morph category (unused by AM)
			return false;
		mci->Category = (MorphDefs::MorphCategory)cat;

		unsigned char type;
		if (fread_s(&type, sizeof(unsigned char), 1, 1, f) < 1) //get the morph type
			return false;
		mci->Type = (MorphDefs::MorphType)type;

		if (fread_s(&mci->OffsetCount, 4, 1, 4, f) < 4) //get the number of offset information blocks
			return false;

		//allocate the offset array based on morph type, others are nulled in AMMorph constructor
		if (mci->Type == MorphDefs::Group)
			mci->Groups = new MorphGroup[mci->OffsetCount];
		else if (mci->Type == MorphDefs::Vertex)
			mci->Vertices = new MorphVertex[mci->OffsetCount];
		else if (mci->Type == MorphDefs::Bone)
			mci->Bones = new MorphBone[mci->OffsetCount];
		else if (mci->Type == MorphDefs::UV || mci->Type == MorphDefs::AUV1 || mci->Type == MorphDefs::AUV2 || mci->Type == MorphDefs::AUV3 || mci->Type == MorphDefs::AUV4)
			mci->UVs = new MorphUV[mci->OffsetCount];
		else if (mci->Type == MorphDefs::Material)
			mci->Materials = new MorphMaterial[mci->OffsetCount];
		else
			return false;

		int ind;
		for (int o=0;o<mci->OffsetCount;o++)
		{
			if (mci->Type == MorphDefs::Group)
			{
				ind = indexConverter(_faceIdSize);
				mci->Groups[o].GroupedMorph = &_morphList[ind]; //get grouped morph
				if (fread_s(&mci->Groups[o].MorphRate, 4, 1, 4, f) < 4) //get morph rate
					return false;
			}
			else if (mci->Type == MorphDefs::Vertex)
			{
				ind = indexConverter(_indexSize);
				mci->Vertices[o].Vertex = &cVertices[ind]; //get the vertex
				if (fread_s(&mci->Vertices[o].MaxOffset, 12, 1, 12, f) < 12) //get the max position offset
					return false;
			}
			else if (mci->Type == MorphDefs::Bone)
			{
				ind = indexConverter(_boneIdSize);
				mci->Bones[o].Bone = &bl[ind]; //get the bone
				if (fread_s(&mci->Bones[o].MaxOffset, 12, 1, 12, f) < 12) //get the max translation offset
					return false;
				if (fread_s(&mci->Bones[o].MaxRotation, 16, 1, 16, f) < 16) //get the max rotation offset
					return false;
			}
			else if (mci->Type == MorphDefs::UV || mci->Type == MorphDefs::AUV1 || mci->Type == MorphDefs::AUV2 || mci->Type == MorphDefs::AUV3 || mci->Type == MorphDefs::AUV4)
			{
				ind = indexConverter(_indexSize);
				mci->UVs[o].Vertex = &cVertices[ind]; //get the vertex
				D3DXVECTOR4 ofs; //store offset in temp variable b/c the file stores 4 values, but we only need 2 for UV coords
				if (fread_s(&ofs, 16, 1, 16, f) < 16) //get the offset
					return false;
				mci->UVs[o].MaxOffset.x = ofs.x; //copy the UV coords over
				mci->UVs[o].MaxOffset.y = ofs.y;
			}
			else if (mci->Type == MorphDefs::Material)
			{
				ind = indexConverter(_matIdSize);
				mci->Materials[o].Material = &_material[ind]; //get the material
				unsigned char t;
				if (fread_s(&t, 1, 1, 1, f) < 1) //get the calculation type (multiply or add)
					return false;
				mci->Materials[o].OffsetType = (MorphDefs::MorphMatType)t;
				if (fread_s(&mci->Materials[o].Diffuse, 16, 1, 16, f) < 16) //get the diffuse color
					return false;
				if (fread_s(&mci->Materials[o].Specular, 12, 1, 12, f) < 12) //get the specular color
					return false;
				if (fread_s(&mci->Materials[0].Shininess, 4, 1, 4, f) < 4) //get the specular coefficient (shininess)
					return false;
				if (fread_s(&mci->Materials[o].Ambient, 12, 1, 12, f) < 12) //get the ambient color
					return false;
				if (fread_s(&mci->Materials[o].EdgeColor, 16, 1, 16, f) < 16) //get the edge color
					return false;
				if (fread_s(&mci->Materials[0].EdgeSize, 4, 1, 4, f) < 4) //get the edge size
					return false;
				if (fread_s(&mci->Materials[o].TextureCoef, 16, 1, 16, f) < 16) //get the texture coefficient
					return false;
				if (fread_s(&mci->Materials[o].SphereCoef, 16, 1, 16, f) < 16) //get the sphere coefficient
					return false;
				if (fread_s(&mci->Materials[o].ToonCoef, 16, 1, 16, f) < 16) //get the toon coefficient
					return false;
			}
		}
	}

	//display frames, we can just skip this
	unsigned long nf;
	if (fread_s(&nf, 4, 1, 4, f) < 4)
		return false;

	for (int i=0;i<(int)nf;i++)
	{
		getString(&tmp);
		delete[] tmp;
		getString(&tmp);
		delete[] tmp;

		char special;
		if (fread_s(&special, 1, 1, 1, f) < 1)
			return false;

		unsigned long ne;
		if (fread_s(&ne, 4, 1, 4, f) < 4)
			return false;
		for (int e=0;e<(int)ne;e++)
		{
			char type;
			if (fread_s(&type, 1, 1, 1, f) < 1)
				return false;
			if (type == 0)
				indexConverter(_boneIdSize);
			else if (type == 1)
				indexConverter(_faceIdSize);
			else
				return false;
		}
	}

	//rigid bodies
	if (fread_s(&_numRigidBody, 4, 1, 4, f) < 4)
		return false;
	_rigidBodyList = new AMRigidBody[_numRigidBody];

	RigidBodyInfo* rci;
	for (int i=0;i<(int)_numRigidBody;i++)
	{
		rci = _rigidBodyList[i].getInfo();

		getString(&rci->Name);
		getString(&rci->NameEnglish);

		int relatedBone = indexConverter(_boneIdSize);
		if (relatedBone == -1)
		{
			rci->RelatedBone = boneController.getCenterBone();
		}
		else
		{
			rci->RelatedBone = &bl[relatedBone];
		}

		if (fread_s(&rci->Group, 1, 1, 1, f) < 1) //get the collision group
			return false;

		if (fread_s(&rci->NonCollisionMask, 2, 1, 2, f) < 2) //get the non-collision mask
			return false;

		char shape;
		if (fread_s(&shape, 1, 1, 1, f) < 1) //get the body shape
			return false;
		rci->Shape = (RigidBodyDefs::RigidBodyShape)shape;

		if (fread_s(&rci->BodySize, 12, 1, 12, f) < 12) //get the body size
			return false;

		if (fread_s(&rci->RelativePos, 12, 1, 12, f) < 12) //get the body position
			return false;
		rci->RelativePos -= rci->RelatedBone->getInfo()->InitialPosition; //correction since pmx does everything in world coordinates

		if (fread_s(&rci->RelativeRot, 12, 1, 12, f) < 12) //get the body rotation
			return false;

		if (fread_s(&rci->PhysConstants.Mass, 4, 1, 4, f) < 4) //mass
			return false;
		if (fread_s(&rci->PhysConstants.LinearDamping, 4, 1, 4, f) < 4) //linear attenuation
			return false;
		if (fread_s(&rci->PhysConstants.AngularDamping, 4, 1, 4, f) < 4) //rotational attenuation
			return false;
		if (fread_s(&rci->PhysConstants.Restitution, 4, 1, 4, f) < 4) //repulsion
			return false;
		if (fread_s(&rci->PhysConstants.Friction, 4, 1, 4, f) < 4) //friction
			return false;

		char type;
		if (fread_s(&type, 1, 1, 1, f) < 1) //get the body type
			return false;
		rci->Type = (RigidBodyDefs::RigidBodyType)type;

		_rigidBodyList[i].init(_pPhysics->getWorld());
	}

	//physics constraints
	if (fread_s(&_numConstraint, 4, 1, 4, f) < 4)
		return false;
	_constraintList = new AMConstraint[_numConstraint];

	ConstraintInfo* cci;
	for (int i=0;i<(int)_numConstraint;i++)
	{
		cci = _constraintList[i].getInfo();

		getString(&cci->Name);
		getString(&cci->NameEnglish);

		char type;
		if (fread_s(&type, 1, 1, 1, f) < 1) //just ignore type, since pmx v2.0 is only spring 6dof
			return false;

		//get the bodies held by this constraint
		int indexA = indexConverter(_rigidBodyIdSize);
		int indexB = indexConverter(_rigidBodyIdSize);
		if (indexA >= 0)
			cci->BodyA = &_rigidBodyList[indexA];
		if (indexB >= 0)
			cci->BodyB = &_rigidBodyList[indexB];

		if (fread_s(&cci->InitialPos, 12, 1, 12, f) < 12) //get position
			return false;
		if (fread_s(&cci->InitialRot, 12, 1, 12, f) < 12) //get rotation
			return false;

		if (fread_s(&cci->PosLimitLower, 12, 1, 12, f) < 12) //get pos lower limit
			return false;
		if (fread_s(&cci->PosLimitUpper, 12, 1, 12, f) < 12) //get pos upper limit
			return false;
		if (fread_s(&cci->RotLimitLower, 12, 1, 12, f) < 12) //get rot lower limit
			return false;
		if (fread_s(&cci->RotLimitUpper, 12, 1, 12, f) < 12) //get rot upper limit
			return false;

		if (fread_s(&cci->Stiffness, 24, 1, 24, f) < 24) //get pos and rot spring stiffness
			return false;

		_constraintList[i].init(_pPhysics->getWorld());
	}


	prepareD3D();

	return true;
}