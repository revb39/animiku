/************************************************************************
*	AM_AMD.h -- AniMiku Motion Data file parser							*
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

#ifndef _AMD_H_
#define _AMD_H_
#include "AM_Motion.h"

/*Size of a bone keyframe block (defined in AMD spec)*/
#define BONE_KEYFRAME_BLOCK_SIZE 96
/*Size of a facial morph keyframe block (defined in AMD spec)*/
#define FACE_KEYFRAME_BLOCK_SIZE 8

/*
	Inherits from AM_Motion
	Implements parse() to support loading AMD file type

	The AMD file spec allows for efficient parsing by simply mapping data structures
	in code to arrays of data in the raw file data.
*/
class AM_Amd : public AM_Motion {
private:
	/*Number of bones used in the animation*/
	unsigned long _numBone;
	/*Number of facial morphs in the animation*/
	unsigned long _numFace;
	/*MD array: bone list->keyframes per bone*/
	KEY_BONE **_boneData;
	/*MD array: facial morph list->keyframes per facial morph*/
	KEY_FACE **_faceData;

	/*
		virtual bool parse()

		Parameters:		None
		Returns:		bool		true/false - parsing was sucessfull
	*/
	virtual bool parse();

public:
	AM_Amd();
	~AM_Amd();
};
#endif