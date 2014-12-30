/************************************************************************
*	MotionController.cpp -- Timing and control of motions and animation	*
*							Copyright (c) 2014 Justin Byers				*
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

#include "MotionController.h"

MotionController::MotionController()
{
	numBones = 0;
	numFrames = 0;
	boneTracks = NULL;
	faceTracks = NULL;
	latestKeyframe = 0;
	needsReset = false;
}
MotionController::~MotionController()
{
	if (boneTracks)
		delete[] boneTracks;
	if (faceTracks)
		delete[] faceTracks;
}

void MotionController::setup(int fps, int numBone, int numFace)
{
	//assign data fps
	_fps = fps;
	//set up bone track list
	this->numBones = numBone;
	if (numBone > 0)
		boneTracks = new BONE_TRACK[numBone];
	//set up morph track list
	this->numFace = numFace;
	if (numFace > 0)
		faceTracks = new FACE_TRACK[numFace];
}
void MotionController::RegisterBoneKeyframes(int boneID, char* boneName, int numKeyframes, KEY_BONE *keys)
{
	//assign information in track
	boneTracks[boneID].keys = keys;
	boneTracks[boneID].curKeyframe = 1;
	boneTracks[boneID].numKeyframes = numKeyframes;
	strcpy_s(boneTracks[boneID].name, sizeof(boneTracks[boneID].name), boneName);

	//update frame counter if last keyframe frame # > current frame counter
	if (boneTracks[boneID].keys[numKeyframes-1].frame > numFrames)
		numFrames = boneTracks[boneID].keys[numKeyframes-1].frame;
}
void MotionController::RegisterFaceKeyframes(int faceID, char* faceName, int numKeyframes, KEY_FACE *keys/*KEY_FACE* keys*/)
{
	//assign information in track
	faceTracks[faceID].keys = keys;
	faceTracks[faceID].curKeyframe = 1;
	faceTracks[faceID].numKeyframes = numKeyframes;
	strcpy_s(faceTracks[faceID].name, sizeof(faceTracks[faceID].name), faceName);

	//update frame counter if last keyframe frame # > current frame counter
	if (faceTracks[faceID].keys[numKeyframes-1].frame > numFrames)
		numFrames = faceTracks[faceID].keys[numKeyframes-1].frame;
}
void MotionController::UpdatePosAndRot(char* boneName, float time, int boneID, D3DXVECTOR3* pos, D3DXQUATERNION* rot)
{
	//if the bone ID is invalid or name doesn't match
	//attempt to find the bone by searching for the name
	if (boneID >= numBones || strcmp(boneName, boneTracks[boneID].name))
	{
		bool found = false;
		//search for bone name in list
		for (int b=0;b<numBones;b++)
		{
			if (!strcmp(boneName, boneTracks[b].name))
			{
				boneID = b;
				found = true;
				break;
			}
		}
		//if can't find bone, don't do anything
		if (!found)
			return;
	}
	//get the animation track & info for this bone
	BONE_TRACK boneTrack = boneTracks[boneID];
	int boneCurKeyframe = boneTrack.curKeyframe;
	int boneNumKeyframe = boneTrack.numKeyframes;
	KEY_BONE *keys = boneTrack.keys;

	//get the last keyframe in this animation track
	KEY_BONE lastKey;
	//curKeyframe should be 1-based, but if for some reason it's not...
	if (boneCurKeyframe == 0)
		lastKey = keys[boneCurKeyframe];
	else
		lastKey = keys[boneCurKeyframe-1];

	//if past end of animation, return data from last keyframe
	if (boneCurKeyframe >= boneNumKeyframe)
	{
		*pos = lastKey.pos;
		*rot = lastKey.rot;
		return;
		
	}

	//get the next keyframe in the animation
	KEY_BONE nextKey = keys[boneCurKeyframe];
	//calculate current frame # from provided time and FPS
	int curFrame = (int)(time*_fps);
	//increment curKeyframe if time passed next defined keyframe
	if (curFrame >= nextKey.frame)
	{
		//increment keyframe counter
		boneTracks[boneID].curKeyframe++;
		boneCurKeyframe++;
		//get previous keyframe of new keyframe
		lastKey = keys[boneCurKeyframe - 1];
		//if just passed final keyframe, return data from last keyframe
		if (boneCurKeyframe >= boneNumKeyframe)
		{
			*pos = lastKey.pos;
			*rot = lastKey.rot;
			return;
		}
		else
		{
			//otherwise just get next keyframe
			nextKey = keys[boneCurKeyframe];
		}
	}

	//get frame numbers of last and next keyframe
	int lastKeyframeFrame = lastKey.frame;
	int nextKeyframeFrame = nextKey.frame;

	//check if need to update latest frame number
	if (lastKeyframeFrame > latestKeyframe)
	{
		latestKeyframe = lastKeyframeFrame;
	}

	//calculate distance (in frames) between next and last keyframes
	int totDist = nextKeyframeFrame - lastKeyframeFrame;
	//if this happens something done fucked up
	if (totDist == 0)
		return;

	//get the distance from current frame (based on time) to last keyframe frame #
	int distLastToCur = curFrame - lastKeyframeFrame;
	//get percentage of distance current frame is from last keyframe to next keyframe
	float s = ((float)distLastToCur/(float)totDist);
	//check limits
	if (s > 1)
		s = 1;

	//solve all bezier curves for interpolated percentage
	float intX, intY, intZ, intRot;
	intX = AMBezier::solveForY(s, nextKey.interpol[0][0], nextKey.interpol[0][1], nextKey.interpol[0][2], nextKey.interpol[0][3]);
	intY = AMBezier::solveForY(s, nextKey.interpol[1][0], nextKey.interpol[1][1], nextKey.interpol[1][2], nextKey.interpol[1][3]);
	intZ = AMBezier::solveForY(s, nextKey.interpol[2][0], nextKey.interpol[2][1], nextKey.interpol[2][2], nextKey.interpol[2][3]);
	intRot = AMBezier::solveForY(s, nextKey.interpol[3][0], nextKey.interpol[3][1], nextKey.interpol[3][2], nextKey.interpol[3][3]);

	//do basic linear interpolation with the interpolated
	//percentage to get final position and rotation
	pos->x = (1-intX)*lastKey.pos.x + (intX*nextKey.pos.x);
	pos->y = (1-intY)*lastKey.pos.y + (intY*nextKey.pos.y);
	pos->z = (1-intZ)*lastKey.pos.z + (intZ*nextKey.pos.z);
	D3DXQuaternionSlerp(rot, &lastKey.rot, &nextKey.rot, intRot);

	//not off the first frame, so the animation can be reset
	needsReset = true;
}

void MotionController::UpdateFaceWeights(char* faceName, float time, int faceID, float* weight)
{
	//no morphs? just say that no weight
	if (numFace == 0)
	{
		*weight = 0;
		return;
	}
	//if the morph ID is invalid or name doesn't match
	//attempt to find the morph by searching for the name
	if (faceID > numFace || strcmp(faceName, faceTracks[faceID].name))
	{
		bool found = false;
		//search for the morph
		for (int b=0;b<numFace;b++)
		{
			if (!strcmp(faceName, faceTracks[b].name))
			{
				faceID = b;
				found = true;
				break;
			}
		}
		//if couldn't find the morph, just return
		if (!found)
			return;
	}

	//get the morph animation track and its info
	FACE_TRACK faceTrack = faceTracks[faceID];
	int faceCurKeyframe = faceTrack.curKeyframe;
	int faceNumKeyframe = faceTrack.numKeyframes;
	KEY_FACE *keys = faceTrack.keys;
	//get the last keyframe
	KEY_FACE lastKey = keys[faceCurKeyframe-1];

	if (faceCurKeyframe >= faceNumKeyframe)
	{
		//if the keyframe we are currently interpolating to does not exist,
		//return the weight value from the previous keyframe
		*weight = lastKey.weight;
		return;
	}

	//get the next keyframe
	KEY_FACE nextKey = keys[faceCurKeyframe];

	int curFrame = (int)(time*_fps);
	if (curFrame > nextKey.frame)
	{
		//if the current frame is greater then the keyframe we are currently interpolating to,
		//increment to the next keyframe
		faceTracks[faceID].curKeyframe++;
		faceCurKeyframe++;
		//get the new last keyframe
		lastKey = keys[faceCurKeyframe-1];
		if (faceCurKeyframe >= faceNumKeyframe)
			nextKey = lastKey;
		else
			nextKey = keys[faceCurKeyframe];
	}

	//get the frame of the last and next keyframe
	int lastKeyframeFrame = lastKey.frame;
	int nextKeyframeFrame = nextKey.frame;

	//check if need to update latest frame number
	if (lastKeyframeFrame > latestKeyframe)
	{
		latestKeyframe = lastKeyframeFrame;
	}

	//get total distance (in frames) between next and last keyframes
	int totDist = nextKeyframeFrame - lastKeyframeFrame;
	//again, if this happens something done fucked up
	if (totDist <= 0)
		return;

	//get the distance between current frame (based on time) and last keyframe
	int distLastToCur = curFrame - lastKeyframeFrame;

	//calculate percent of current frame from last keyframe to next keyframe
	float s = (float)distLastToCur/(float)totDist;
	//check ranges
	if (s > 1 || s < 0)
	{
		*weight = 0;
		return;
	}

	//use temporary vectors to calculate interpolated weight value
	D3DXVECTOR3 tempLst = D3DXVECTOR3(lastKey.weight, 0, 0);
	D3DXVECTOR3 tempNxt = D3DXVECTOR3(nextKey.weight, 0, 0);
	D3DXVECTOR3 temp;
	D3DXVec3Lerp(&temp, &tempLst, &tempNxt, s);
	//check ranges again
	if (temp.x > 1)
	{
		*weight = 0;
		return;
	}
	//return the weight
	*weight = temp.x;
}
void MotionController::GetDefBonePos(char* boneName, int boneID, D3DXVECTOR3* pos, D3DXQUATERNION* rot)
{
	//if the bone ID is invalid or name doesn't match
	//attempt to find the bone by searching for the name
	if (boneID >= numBones || strcmp(boneName, boneTracks[boneID].name))
	{
		bool found = false;
		//search for the bone
		for (int b=0;b<numBones;b++)
		{
			if (!strcmp(boneName, boneTracks[b].name))
			{
				boneID = b;
				found = true;
				break;
			}
		}
		//if couldn't find it just exit here
		if (!found)
			return;
	}
	//get pos and rot from first keyframe
	*pos = boneTracks[boneID].keys[0].pos;
	*rot = boneTracks[boneID].keys[0].rot;
}
void MotionController::GetDefFaceWeights(char* faceName, int faceID, float* weight)
{
	//if the morph ID is invalid or name doesn't match
	//attempt to find the morph by searching for the name
	if (faceID > numFace || strcmp(faceName, faceTracks[faceID].name))
	{
		bool found = false;
		//search for the morph
		for (int b=0;b<numFace;b++)
		{
			if (!strcmp(faceName, faceTracks[b].name))
			{
				faceID = b;
				found = true;
				break;
			}
		}
		//if couldn't find it just exit here
		if (!found)
			return;
	}
	//get weight from first keyframe
	*weight = faceTracks[faceID].keys[0].weight;
}
double MotionController::GetLengthInSeconds()
{
	return (double)numFrames / (double)_fps;
}
bool MotionController::done()
{
	return (latestKeyframe == numFrames);
}
void MotionController::ResetMotion()
{
	if (!needsReset)
		return;
	//reset counters to first keyframe
	for (int i=0;i<numBones;i++)
	{
		boneTracks[i].curKeyframe = 1;
	}
	for (int i=0;i<numFace;i++)
	{
		faceTracks[i].curKeyframe = 1;
	}
}