/************************************************************************
*	AM_Motion.h -- Motion data loading and playback						*
*				   Copyright (c) 2014 Justin Byers						*
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

#ifndef _MOTION_H_
#define _MOTION_H_

#include "AMTypes.h"
#include "MotionController.h"

/*Magic key for AMD conversion*/
#define AMD_MAGIC "amd"
/*AMD format version for AMD conversion*/
#define AMD_VERSION 1.0f

/*Map Japanese bone name to array of bone keyframes*/
typedef map<string, KEY_BONE*> BoneKeyList;
/*Map Japanese morph name to array of morph keyframes*/
typedef map<string, KEY_FACE*> FaceKeyList;
/*Map Japanese bone/morph name to number of keyframes in that bone/morph*/
typedef map<string, int> KeySizeList;

class AM_Motion;
struct MotionType
{
	char* Description;
	void* Constructor;
};
/*Just call RegisterMotionType to link a file extension to a class constructor*/
#define RegisterMotionType(i,d,t) (AM_Motion::MotionParsers()[i].Description = d);(AM_Motion::MotionParsers()[i].Constructor = &createInstanceMotion<t>)
template<class T> AM_Motion* createInstanceMotion() {return new T;}
typedef map<string, MotionType> InhMotionParsers;

/*
	Handles all aspects of preparing and playing motion data
*/
class AM_Motion {
protected:
	/*All bone keyframes in motion data*/
	BoneKeyList _boneKeyList;
	/*All sizes of all bone data in motion data*/
	KeySizeList _boneKeySizeList;
	/*Iterator for iterating through all bones in motion data*/
	BoneKeyList::iterator _bit;

	/*All morph keyframes in motion data*/
	FaceKeyList _faceKeyList;
	/*All sizes of all morph data in motion data*/
	KeySizeList _faceKeySizeList;
	/*Iterator for iterating through all morphs in motion data*/
	FaceKeyList::iterator _fit;

	/*FILE object for the motion data file*/
	FILE *_f;

	/*Pointer to this motion data's motion controller*/
	MotionController *_motionController;

	/*Playback FPS of the motion data*/
	int _fps;

private:
	/*Directory of motion data file*/
	char* _dir;
	/*Filename of motion data (not full path)*/
	char* _fileName;

	/*Time playback started*/
	DWORD _startTime;
	/*Current time in motion data*/
	float _curTime;
	/*Motion data is playing*/
	bool _playing;
	/*Playback has been stopped*/
	bool _stoppedFlag;

	/*
		virtual bool parse()

		Implemented by subclass to parse different motion data file types

		Returns:		bool			Parsing was sucessfull
	*/
	virtual bool parse();

public:
	AM_Motion();
	virtual ~AM_Motion();

	/*
		bool dumpToAMD()

		Export the loaded motion data in the AMD format

		Returns:		bool			Export was sucessfull
	*/
	bool dumpToAMD();

	/*
		bool load()

		Load and parse a motion data file
	*/
	bool load(char* file);
	/*
		void start()

		Start playback of motion data at beginning of motion
	*/
	void start();
	/*
		void stop()

		Stop playback of motion data. Does not reset to beginning of data
	*/
	void stop();
	/*
		bool update()

		Update playback to current real time		
	*/
	void update();
	/*
		bool getIsPlaying()

		Get if the motion is currently playing

		Returns:		bool			The motion is currently playing
	*/
	bool getIsPlaying();
	/*
		bool getStopped()

		Get if playback has been stopped. Returns true once, resets on next play

		Returns:		bool			Playback has been stopped
	*/
	bool getStopped();

	/*
		void getPosAtTimeFromBone()

		Get the position and rotation of a bone at current time from motion

		Parameters:		boneName		Name (in Japanese) of bone to get data from
						boneID			ID of the bone to get data from
						pos				Pointer to the D3DXVECTOR3 to receive position data
						rot				Pointer to the D3DXQUATERNION to receive rotation data
	*/
	void getPosRotAtTimeFromBone(char* boneName, int boneID, D3DXVECTOR3* pos, D3DXQUATERNION* rot);
	/*
		void getWeightAtTimeFromFace()

		Get the weight of a morph at current time from motion

		Parameters:		faceName		Name (in Japanese) of morph to get weight from
						faceID			ID of the morph to get weight from
						weight			Pointer to the float to receive the weight
	*/
	void getWeightAtTimeFromFace(char* faceName, int faceID, float* weight);

	/*
		void getDefPose()

		Get the bind pose pos and rot of a bone

		Parameters:		boneName		Name (in Japanese) of bone to get data from
						boneID			ID of the bone to get data from
						pos				Pointer to the D3DXVECTOR3 to receive position data
						rot				Pointer to the D3DXQUATERNION to receive rotation data
	*/
	void getDefPose(char* boneName, int boneID, D3DXVECTOR3* pos, D3DXQUATERNION* rot);
	/*
		void getDefFace()

		Get the bine pose weight of a morph

		Parameters:		faceName		Name (in Japanese) of the morph to get data from
						faceID			ID of the morph to get data from
						weight			Pointer to the float to receive the weight
	*/
	void getDefFace(char* faceName, int faceID, float* weight);

	static InhMotionParsers &MotionParsers()
	{
		static InhMotionParsers mp;
		return mp;
	}
	static AM_Motion* CreateFromType(char* type);
};
#endif