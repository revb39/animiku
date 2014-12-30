/************************************************************************
*	AM_VMD.cpp -- File parsing for VMD format							*
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

#include "AM_VMD.h"
AM_Vmd::AM_Vmd()
{
	_fps = VMD_UPSAMPLE_FPS; //VMD will be upsampled since 30fps animations look like shit
}

AM_Vmd::~AM_Vmd()
{
	for (_pBit=_pBoneKeyList.begin();_pBit!=_pBoneKeyList.end();++_pBit)
	{
		for (int i=0;i<(int)(*_pBit).second.size();i++)
		{
			if ((*_pBit).second[i])
				delete (*_pBit).second[i];
		}
		if (_boneKeyList[(*_pBit).first])
			delete[] _boneKeyList[(*_pBit).first];
	}
	for (_pFit=_pFaceKeyList.begin();_pFit!=_pFaceKeyList.end();++_pFit)
	{
		for (int i=0;i<(int)(*_pFit).second.size();i++)
		{
			if ((*_pFit).second[i])
				delete (*_pFit).second[i];
		}
		if (_faceKeyList[(*_pFit).first])
			delete[] _faceKeyList[(*_pFit).first];
	}
}
//keyframe sorting functions
bool AM_Vmd::compareBone(const KEY_BONE* a, const KEY_BONE* b)
{
	return a->frame < b->frame;
}
bool AM_Vmd::compareFace(const KEY_FACE* a, const KEY_FACE* b)
{
	return a->frame < b->frame;
}
bool AM_Vmd::parse()
{
	//get the identifier and validate that this is a true VMD
	char id[30];
	if (fread_s(id, 30, 1, 30, _f) < 30)
		return false;
	if (strcmp(id, "Vocaloid Motion Data 0002"))
		return false;
	
	//name is unneeded
	char name[20];
	if (fread_s(name, 20, 1, 20, _f) < 20)
		return false;

	//calculate scalar value for upsampling
	float fpsScl = VMD_UPSAMPLE_FPS/30.0f;

	//get all bone keyframes
	fread(&_numBoneKeyframe, sizeof(unsigned long), 1, _f);
	for (int i=0;i<(int)_numBoneKeyframe;i++)
	{
		//get the keyframe data
		VMD_BoneFrame curBoneFrame;
		fread(&curBoneFrame, sizeof(curBoneFrame), 1, _f);

		//create a new keyframe object for this keyframe
		KEY_BONE *boneKey = new KEY_BONE();
		//apply the upscale by scaling the frame number
		//the motion controller will automatically interpolate the blank space
		boneKey->frame = (int)(curBoneFrame.keyFrame*fpsScl);
		//get the pos and rot
		boneKey->pos = curBoneFrame.pos;
		boneKey->rot = curBoneFrame.rot;
		//get the bezier interpolation data
		for (int b=0;b<4;b++)
		{
			float x1 = curBoneFrame.interpolation[b];
			float y1 = curBoneFrame.interpolation[4+b];
			float x2 = curBoneFrame.interpolation[8+b];
			float y2 = curBoneFrame.interpolation[12+b];
			x1 /= 127.0f;
			y1 /= 127.0f;
			x2 /= 127.0f;
			y2 /= 127.0f;
			boneKey->interpol[b][0] = x1;
			boneKey->interpol[b][1] = y1;
			boneKey->interpol[b][2] = x2;
			boneKey->interpol[b][3] = y2;
		}

		//finally add the keyframe to the list
		_pBoneKeyList[curBoneFrame.name].push_back(boneKey);
	}

	//get all morph keyframes
	fread(&_numFaceKeyframe, sizeof(unsigned long), 1, _f);
	for (int i=0;i<(int)_numFaceKeyframe;i++)
	{
		//get the keyframe data
		VMD_FaceFrame curFaceFrame;
		fread(&curFaceFrame, sizeof(curFaceFrame), 1, _f);
		//create a new keyframe object for this keyframe
		KEY_FACE *faceKey = new KEY_FACE();
		//apply the upscale by scaling the frame number
		//the motion controller will automatically interpolate the blank space
		faceKey->frame = (int)(curFaceFrame.keyFrame*fpsScl);
		//get the weight
		faceKey->weight = curFaceFrame.weight;

		//finall add the keyframe to the list
		_pFaceKeyList[curFaceFrame.name].push_back(faceKey);
	}

	//setup the motion controller for the motion data
	_motionController->setup(_fps, _pBoneKeyList.size(), _pFaceKeyList.size());

	//sort all keyframe lists
	int j=0;
	for (_pBit=_pBoneKeyList.begin();_pBit!=_pBoneKeyList.end();++_pBit)
	{
		sort((*_pBit).second.begin(), (*_pBit).second.end(), compareBone);
		_boneKeyList[(*_pBit).first] = new KEY_BONE[(*_pBit).second.size()];
		_boneKeySizeList[(*_pBit).first] = (*_pBit).second.size();
		for (int k=0;k<(int)(*_pBit).second.size();k++)
		{
			_boneKeyList[(*_pBit).first][k] = *(*_pBit).second[k];
		}
		_motionController->RegisterBoneKeyframes(j, (char*)(*_pBit).first.c_str(), (*_pBit).second.size(), _boneKeyList[(*_pBit).first]);
		j++;
	}
	j = 0;
	for (_pFit=_pFaceKeyList.begin();_pFit!=_pFaceKeyList.end();++_pFit)
	{
		sort((*_pFit).second.begin(), (*_pFit).second.end(), compareFace);
		_faceKeyList[(*_pFit).first] = new KEY_FACE[(*_pFit).second.size()];
		_faceKeySizeList[(*_pFit).first] = (*_pFit).second.size();
		for (int k=0;k<(int)(*_pFit).second.size();k++)
		{
			_faceKeyList[(*_pFit).first][k] = *(*_pFit).second[k];
		}
		_motionController->RegisterFaceKeyframes(j, (char*)(*_pFit).first.c_str(), (*_pFit).second.size(), _faceKeyList[(*_pFit).first]);
		j++;
	}

	return true;
}