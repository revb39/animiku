/************************************************************************
*	AM_IK.h -- IK chain solver											*
*			   Copyright (c) 2014 Justin Byers							*
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

#ifndef _IK_H_
#define _IK_H_
#include "AM_BoneNew.h"

/*Constants for IK solving*/
#define IK_MIN_DIST 0.0001f
#define IK_MIN_ANGLE 0.00000001f
#define IK_MIN_AXIS 0.0000001f
#define IK_MIN_ROT_SUM 0.002f
#define IK_MIN_ROT 0.00001f
#define PI 3.14159265f
/*A link, related to a bone, in the IK chain*/
struct IKLink
{
	AMBone* LinkBone;
	bool AngleLimit;
	D3DXVECTOR3 LowerLimit;
	D3DXVECTOR3 UpperLimit;
	D3DXQUATERNION IKQuat;
};
/*IK information structure*/
struct IKInfo
{
	AMBone* DestinationBone;
	AMBone* EndEffectorBone;
	unsigned long IterationCount;
	float AngleConstraint;
	unsigned long NumberOfLinks;
	IKLink* BoneLinkList; 
};
/*
	Solve IK using MMD's weird ass CCD algorithm implementation
*/
class IK
{
private:
	/*The IK info structure*/
	IKInfo _info;
	/*Is this IK chain simulated in physics?*/
	bool simulated;
	/*
		bool checkSimulated()

		Determine if the IK is simulated. Chain will be considered simulated if any
		of the chain's links' bones are simulated

		Returns:		bool			The chain is simulated
	*/
	bool checkSimulated();

public:
	IK();
	~IK();
	/*
		IKInfo* getInfo()

		Get the IK chain's info structure

		Returns:		Pointer to the chain's info structure
	*/
	IKInfo* getInfo();
	/*
		void init()

		Initialize the IK chain
	*/
	void init();
	/*
		void solve()

		Solve the IK chains
	*/
	void solve();
};
#endif