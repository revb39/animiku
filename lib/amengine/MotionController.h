/************************************************************************
*	MotionController.h -- Timing and control of motions and animation	*
*						  Copyright (c) 2014 Justin Byers				*
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

#ifndef MOCONTROL_H_
#define MOCONTROL_H_

#include "AMTypes.h"
#include "AM_Bezier.h"
using namespace std;

/*A bone keyframe*/
struct KEY_BONE {
	int frame;
	D3DXVECTOR3 pos;
	D3DXQUATERNION rot;
	float interpol[4][4];
};
/*A morph keyframe*/
struct KEY_FACE {
	int frame;
	float weight;
};
/*Animation track for a single bone*/
struct BONE_TRACK {
	int numKeyframes;
	int curKeyframe;
	KEY_BONE *keys;
	char name[30];
};
/*Animation track for a single morph*/
struct FACE_TRACK {
	int numKeyframes;
	int curKeyframe;
	KEY_FACE *keys;
	char name[30];
};

/*
	Handles all calculations needed for animating and interpolation
*/
class MotionController {
private:
	/*The motion can be reset*/
	bool needsReset;
	/*Number of bones in the motion data*/
	int numBones;
	/*Number of morphs in the motion data*/
	int numFace;
	/*Length of motion data in frames*/
	int numFrames;
	/*The latest keyframe passed while animating*/
	int latestKeyframe;
	/*FPS of the animation data*/
	int _fps;

	/*Array of bone animation tracks*/
	BONE_TRACK *boneTracks;
	/*Array of morph animation tracks*/
	FACE_TRACK *faceTracks;
	
public:
	MotionController();
	~MotionController();

	/*
		void setup()

		Set up and initialize the motion controller

		Parameters:			fps			FPS of the motion data
							numBone		Number of bones in motion data
							numFace		Number of morphs in motion data
	*/
	void setup(int fps, int numBone, int numFace);
	/*
		void RegisterBoneKeyframes()

		Register an array of bone keyframes in the motion controller

		Parameters:			boneID			ID of the bone
							boneName		Name of the bone
							numKeyframes	Number of keyframes on this bone
							keys			Array of keyframes
	*/
	void RegisterBoneKeyframes(int boneID, char* boneName, int numKeyframes, KEY_BONE *keys);
	/*
		void RegisterFaceKeyframes()

		Register an array of morph keyframes in the motion controller

		Parameters:			faceID			ID of the morph
							faceName		Name of the morph
							numKeyframes	Number of keyframes on this morph
							keys			Array of keyframes
	*/
	void RegisterFaceKeyframes(int faceID, char* faceName, int numKeyframes, KEY_FACE *keys);

	/*
		void UpdatePosAndRot()

		Update bone animation track to given time in motion and get pos & rot at that time

		Parameters:			boneName		Name of the bone
							time			Time (in seconds) to update to
							boneID			ID of the bone
							pos				Pointer to D3DXVECTOR3 to receive position data
							rot				Pointer to D3DXQUATERNION to receive rotation data
	*/
	void UpdatePosAndRot(char* boneName, float time, int boneID, D3DXVECTOR3* pos, D3DXQUATERNION* rot);
	/*
		void UpdateFaceWeights()

		Update morph animation track to given time in motion & get weight at that time

		Parameters:			faceName		Name of the morph
							time			Time (in seconds) to update to
							faceID			ID of the morph
							weight			Pointer to float to receive weight data
	*/
	void UpdateFaceWeights(char* faceName, float time, int faceID, float* weight);

	/*
		void GetDefBonePos()

		Get pos and rot of bone at first keyframe in animation

		Parameters:			boneName		Name of the bone
							boneID			ID of the bone
							pos				Pointer to D3DXVECTOR3 to receive pos data
							rot				Pointer to D3DXQUATERNION to receive rotation data
	*/
	void GetDefBonePos(char* boneName, int boneID, D3DXVECTOR3* pos, D3DXQUATERNION* rot);
	/*
		void GetDefFaceWeights()

		Get weight of morph at first keyframe in animation

		Parameters:			faceName		Name of the morph
							faceID			ID of the face
							weight			Pointer to float to receive weight data
	*/
	void GetDefFaceWeights(char* faceName, int faceID, float* weight);
	/*
		void GetLengthInSeconds()

		Get length of animation in seconds

		Returns:			double			Length in seconds
	*/
	double GetLengthInSeconds();
	/*
		void done()

		Get if animation has finished playing

		Returns:			Animation has finished running
	*/
	bool done();
	/*
		void ResetMotion()

		Reset the animation to the first frame
	*/
	void ResetMotion();
};

#endif