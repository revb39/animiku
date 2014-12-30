/************************************************************************
*	AM_VMD.h -- File parsing for VMD format								*
*				Copyright (c) 2014 Justin Byers							*
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

#ifndef _VMD_H_
#define _VMD_H_

#include <algorithm>
#include "AM_Motion.h"

/*FPS to upsample VMD motion data to*/
#define VMD_UPSAMPLE_FPS 120

/*
	Extends AM_Motion to parse VMD motion data
*/
class AM_Vmd : public AM_Motion {
private:
	/*Maps Japanese bone name to array of bone keyframes*/
	map<string, vector<KEY_BONE*>> _pBoneKeyList;
	/*Iterator for bone/keyframe list*/
	map<string, vector<KEY_BONE*>>::iterator _pBit;
	/*Maps Japanese morph name to array of morph keyframes*/
	map<string, vector<KEY_FACE*>> _pFaceKeyList;
	/*Iterator for morph/keyframe list*/
	map<string, vector<KEY_FACE*>>::iterator _pFit;
	/*Number of bone keyframes in the motion data*/
	unsigned long _numBoneKeyframe;
	/*Number of morph keyframes in the motion data*/
	unsigned long _numFaceKeyframe;

	/*
		virtual bool parse()

		See definition in AM_Motion
	*/
	virtual bool parse();

public:
	AM_Vmd();
	~AM_Vmd();

	/*
		static bool compareBone()

		Comparator function for sorting bone keyframes
	*/
	static bool compareBone(const KEY_BONE* a, const KEY_BONE* b);
	/*
		static bool compareFace()

		Comparator function for sorting morph keyframes
	*/
	static bool compareFace(const KEY_FACE* a, const KEY_FACE* b);
};
#endif