/************************************************************************
*	AM_Motion.cpp -- Motion data loading and playback					*
*					 Copyright (c) 2014 Justin Byers					*
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

#include "AM_Motion.h"
#include "AM_BoneNew.h"
#include "AM_Morph.h"
#include "AM_VMD.h"

AM_Motion::AM_Motion()
{
	_fileName = NULL;
	_dir = NULL;
	_f = NULL;
	_motionController = NULL;
	_fps = 0;
	_startTime = 0;
	_curTime = 0;
	_playing = false;
	_stoppedFlag = false;
}
AM_Motion::~AM_Motion()
{
	if (_motionController)
		delete _motionController;
	if (_dir)
		delete[] _dir;
}

bool AM_Motion::dumpToAMD()
{
	//open the file for writing (<original_filename>.amd)
	std::string file = std::string(_dir) + std::string(_fileName) + ".amd";
	FILE* f = NULL;
	fopen_s(&f, file.c_str(), "wb");
	if (f == NULL)
		return false;

	//write the magic key
	if (fwrite(AMD_MAGIC, 1, 3, f) < 3)
		return false;
	//write the format version
	float v = AMD_VERSION;
	if (fwrite(&v, 1, 4, f) < 4)
		return false;
	//write the playback FPS
	unsigned short fps = _fps;
	if (fwrite(&fps, 1, 2, f) < 2)
		return false;
	//write the number of bones
	unsigned long nb = (unsigned long)_boneKeyList.size();
	if (fwrite(&nb, 1, 4, f) < 4)
		return false;
	//write the number of morphs
	unsigned long nm = (unsigned long)_faceKeyList.size();
	if (fwrite(&nm, 1, 4, f) < 4)
		return false;

	//loop through all bones in motion data
	for (BoneKeyList::iterator it = _boneKeyList.begin();it != _boneKeyList.end();++it)
	{
		//AMD format uses UTF8, so convert name
		char *uni = AMUtils::ANSIToUTF8((char*)(*it).first.c_str());
		//write name string length
		unsigned char s = strlen(uni); //string length
		if (fwrite(&s, 1, 1, f) < 1)
			return false;
		//write the name string
		if (fwrite(uni, 1, s, f) < s) //string
			return false;
		delete[] uni;
		//write the number of keyframes for this bone
		unsigned long nk = _boneKeySizeList[(*it).first.c_str()];
		if (fwrite(&nk, 1, 4, f) < 4)
			return false;
		//write each keyframe in this bone
		for (int i=0;i<(int)nk;i++)
		{
			if (fwrite(&(*it).second[i].frame, 1, 4, f) < 4) //frame number
				return false;
			if (fwrite((*it).second[i].pos, 1, 12, f) < 12) //position
				return false;
			if (fwrite((*it).second[i].rot, 1, 16, f) < 16) //rotation
				return false;
			if (fwrite((*it).second[i].interpol, 1, 64, f) < 64) //interpolation data
				return false;
		}
	}

	//loop through all morphs in motion data
	for (FaceKeyList::iterator it = _faceKeyList.begin();it != _faceKeyList.end();++it)
	{
		//AMD format uses UTF8, so convert name
		char* uni = AMUtils::ANSIToUTF8((char*)(*it).first.c_str());
		//write name string length
		unsigned char s = strlen(uni);
		if (fwrite(&s, 1, 1, f) < 1)
			return false;
		//write name string
		if (fwrite(uni, 1, s, f) < s)
			return false;
		//write the number of keyframes for this morph
		unsigned long nk = _faceKeySizeList[(*it).first.c_str()];
		if (fwrite(&nk, 1, 4, f) < 4)
			return false;
		delete[] uni;
		//write each keyframe in this morph
		for (int i=0;i<(int)nk;i++)
		{
			if (fwrite(&(*it).second[i].frame, 1, 4, f) < 4) //frame number
				return false;
			if (fwrite(&(*it).second[i].weight, 1, 4, f) < 4) //weight
				return false;
		}
	}

	//close the file
	fclose(f);

	return true;
}

bool AM_Motion::parse()
{
	//nothing, this needs to be implemented by the inheriting class
	return false;
}
bool AM_Motion::load(char* file)
{
	//open the file for binary read
	fopen_s(&_f, file, "rb");
	if (_f == NULL)
		return false;

	//get directory and filename of opened file
	_dir = AMUtils::getDir(file);
	_fileName = AMUtils::getFile(file);
	//create the motion controller for this motion data
	_motionController = new MotionController();
	
	//attempt to parse the file
	bool ret = parse();
	fclose(_f);
	return ret;
}

void AM_Motion::start()
{
	//reset all counters
	_motionController->ResetMotion();
	_startTime = timeGetTime();
	_curTime = 0;
	_playing = true;
}
void AM_Motion::stop()
{
	_playing = false;
}
void AM_Motion::update()
{
	//get change in time since last update
	float newTime = (timeGetTime()-_startTime)*0.001f;
	float deltaTime = (newTime - _curTime);

	//increment the current time by the time delta
	_curTime += deltaTime;

	if (_curTime >= _motionController->GetLengthInSeconds())
	{
		//if current time is greater than the animation length stop playback
		_playing = false;
		_stoppedFlag = true;
	}
}
void AM_Motion::getPosRotAtTimeFromBone(char* boneName, int boneID, D3DXVECTOR3* pos, D3DXQUATERNION* rot)
{
	_motionController->UpdatePosAndRot(boneName, _curTime, boneID, pos, rot);
}
void AM_Motion::getWeightAtTimeFromFace(char* faceName, int faceID, float* weight)
{
	_motionController->UpdateFaceWeights(faceName, _curTime, faceID, weight);
}
void AM_Motion::getDefPose(char* boneName, int boneID, D3DXVECTOR3* pos, D3DXQUATERNION* rot)
{
	_motionController->GetDefBonePos(boneName, boneID, pos, rot);
}
void AM_Motion::getDefFace(char* faceName, int faceID, float* weight)
{
	_motionController->GetDefFaceWeights(faceName, faceID, weight);
}
bool AM_Motion::getIsPlaying()
{
	return _playing;
}
bool AM_Motion::getStopped()
{
	if (_stoppedFlag)
	{
		_stoppedFlag = false;
		return true;
	}
	return false;
}

AM_Motion* AM_Motion::CreateFromType(char* type)
{
	if (MotionParsers().find(type) == MotionParsers().end())
		return NULL;
	else
		return ((AM_Motion*(*)(void))MotionParsers()[type].Constructor)();
}
