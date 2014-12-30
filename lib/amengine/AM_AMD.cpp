/************************************************************************
*	AM_AMD.cpp -- AniMiku Motion Data file parser						*
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

#include "AM_AMD.h"

AM_Amd::AM_Amd()
{
	//initialize all member variables
	_numBone = 0;
	_numFace = 0;
	_boneData = NULL;
	_faceData = NULL;
}
AM_Amd::~AM_Amd()
{
	//free memory used by bone keyframes
	if (_boneData)
	{
		for (int i=0;i<(int)_numBone;i++)
		{
			if (_boneData[i])
				delete[] _boneData[i];
		}
		delete[] _boneData;
	}
	//free memory used by facial morph keyframes
	if (_faceData)
	{
		for (int i=0;i<(int)_numFace;i++)
		{
			if (_faceData[i])
				delete[] _faceData[i];
		}
		delete[] _faceData;
	}
}

bool AM_Amd::parse()
{
	//read the header and verify correct file type
	char magic[3];
	float version;
	if (fread_s(magic, 3, 1, 3, _f) < 3)
		return false;
	if (fread_s(&version, 4, 1, 4, _f) < 4)
		return false;
	if ((magic[0] != 'a' || magic[1] != 'm' ||magic[2] != 'd') || version != 1.0f)
		return false;

	if (fread_s(&_fps, 4, 1, 2, _f) < 2) //get the fps
		return false;

	//number of bones
	if (fread_s(&_numBone, 4, 1, 4, _f) < 4)
		return false;
	_boneData = new KEY_BONE*[_numBone]; //allocate space for each bone in the raw data array

	//number of faces
	if (fread_s(&_numFace, 4, 1, 4, _f) < 4)
		return false;
	_faceData = new KEY_FACE*[_numFace]; //allocate space for each face in the raw data array

	_motionController->setup(_fps, _numBone, _numFace); //setup the motion controller

	//load the bone keyframes
	for (int i=0;i<(int)_numBone;i++)
	{
		unsigned char s = 0;
		char *nameUni = NULL;
		if (fread_s(&s, 1, 1, 1, _f) < 1) //get string length
			return false;
		nameUni = new char[s+1]; //allocate memory for name
		nameUni[s] = '\0'; //append null terminator
		if (fread_s(nameUni, s, 1, s, _f) < s) //read the name string in UTF8
			return false;
		char *name = AMUtils::UTF8ToANSI(nameUni); //convert the name string from UTF8
		delete[] nameUni;

		unsigned long numKeyframe = 0;
		if (fread_s(&numKeyframe, 4, 1, 4, _f) < 4) //number of keyframes
			return false;
		_boneData[i] = new KEY_BONE[numKeyframe]; //allocate space in the data array

		unsigned int sz = numKeyframe * BONE_KEYFRAME_BLOCK_SIZE;
		if (fread_s(_boneData[i], sz, 1, sz, _f) < sz) //read the entire keyframe data block into memory
			return false;

		//add bone keyframe list and number of keyframes for each bone to AM_Motion
		//only used by AM_Motion to convert to AMD so not really used in this case
		_boneKeyList[name] = _boneData[i];
		_boneKeySizeList[name] = numKeyframe;

		//register all bones and keyframes in the motion controller
		_motionController->RegisterBoneKeyframes(i, name, numKeyframe, _boneData[i]);

		delete[] name;
	}

	//load the face keyframes
	for (int i=0;i<(int)_numFace;i++)
	{
		unsigned char s = 0;
		char* nameUni = NULL;
		if (fread_s(&s, 1, 1, 1, _f) < 1) //get string length
			return false;
		nameUni = new char[s+1]; //allocate memory for name
		nameUni[s] = '\0'; //append null terminator
		if (fread_s(nameUni, s, 1, s, _f) < s) //read the name string in UTF8
			return false;
		char *name = AMUtils::UTF8ToANSI(nameUni); //convert the name string from UTF8
		delete[] nameUni;

		unsigned long numKeyframe = 0;
		if (fread_s(&numKeyframe, 4, 1, 4, _f) < 4) //number of keyframes
			return false;
		_faceData[i] = new KEY_FACE[numKeyframe]; //allocate space in the data array

		unsigned int sz = numKeyframe * FACE_KEYFRAME_BLOCK_SIZE;
		if (fread_s(_faceData[i], sz, 1, sz, _f) < sz) //read the entire keyframe data block into memory
			return false;

		//add facial morph keyframe list and number of keyframes for each bone to AM_Motion
		//only used by AM_Motion to convert to AMD so not really used in this case
		_faceKeyList[name] = _faceData[i];
		_faceKeySizeList[name] = numKeyframe;

		//register all facial morphs and keyframes in the motion controller
		_motionController->RegisterFaceKeyframes(i, name, numKeyframe, _faceData[i]);

		delete[] name;
	}
	return true;
}