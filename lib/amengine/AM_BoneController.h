/************************************************************************
*	AM_BoneController.h -- Manages model skeleton and updating bones	*
*						   Copyright (c) 2014 Justin Byers				*
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

#ifndef AM_BONECONTROLLER_H_
#define AM_BONECONTROLLER_H_

#include "AM_Motion.h"
#include "AM_Constraint.h"

/*Name (in Japanese) of model center bone (defined by MMD spec)*/
#define PMDMODEL_CENTERBONENAME "センター"
/*Name (in Japanese) of model root bone (defined by MMD spec)*/
#define PMDMODEL_ROOTBONENAME "全ての親"

/*
	Acts as the representation of the model's skeleton.
	Manages all updating and calculations of the bone structure.
*/
class AMBoneController
{
private:
	/*List of all bones in the skeleton*/
	AMBone *_bonesList;
	/*Pointer to the center bone*/
	AMBone *_centerBone;
	/*Pointer to the root bone*/
	AMBone *_rootBone;
	/*Was the root bone created by code?*/
	bool generatedRoot;
	/*Number of bones in the skeleton*/
	int _numBones;

	/*List of all IK chains*/
	IK *_ikList;
	/*Number of IK chains in the model*/
	int _numIK;

	/*
		void sortBones()

		Sort the bones, assign children and siblings to each, and initialize each
		bone in the structure
	*/
	void sortBones();
	/*
		void updateTransform()

		Traverse the bone structure and update the transforms of all bones.

		Parameters:		bone			The bone at which to start the traversal
						forceRepos		true/false: always force the bone to be updated	
	*/
	void updateTransform(AMBone *bone, bool forceRepos);
public:
	AMBoneController();
	~AMBoneController();

	/*
		void setupBoneList()

		Initializes the bone structure

		Parameters:		b				List of bones in the bone structure
						numBone			Number of bones in the bone structure
	*/
	void setupBoneList(AMBone *b, unsigned long numBone);

	/*
		void setupIKList()

		Initialize all IK chains

		Parameters:		ik				List of IK chains
						numIK			Number of IK chains
	*/
	void setupIKList(IK* ik, int numIK);

	/*
		void solveIKs()

		Solve all IK chains
	*/
	void solveIKs();

	/*
		void updateTransformsFromRoot()

		Traverse the bone structure and update the transforms of all bones starting
		from root. Skeleton ready for vertex blending after this is called.

		Parameters:		forceRepos		true/false: force all bones to be updated
	*/
	void updateTransformsFromRoot(bool forceRepos);

	/*
		void updateBonesFromMotion()

		Update local transforms of all bones from motion data

		Parameters:		motion			Pointer to the motion data
						isdefPos		true/false: Get the transforms from the bine pose
	*/
	void updateBonesFromMotion(AM_Motion *motion, bool isDefPos);

	/*
		AMBone* getBoneList()

		Get the list of bones in the bone structure

		Returns:		List of bones
	*/
	AMBone* getBoneList();
	/*
		AMBone* getCenterBone()

		Get pointer to the center bone

		Returns:		Pointer to the center bone
	*/
	AMBone* getCenterBone();
};
#endif