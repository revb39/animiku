/************************************************************************
*	AM_BoneController.cpp -- Manages model skeleton and updating bones	*
*							 Copyright (c) 2014 Justin Byers			*
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

#include "AM_BoneController.h"
#include <stack>

AMBoneController::AMBoneController()
{
	//initialize all member variables
	_bonesList = NULL;
	_centerBone = NULL;
	_rootBone = NULL;
	generatedRoot = false;
	_numBones = 0;
	_ikList = NULL;
	_numIK = 0;
}
AMBoneController::~AMBoneController()
{
	if (_bonesList)
		delete[] _bonesList;
	if (_rootBone && generatedRoot)
		delete _rootBone;
	if (_ikList)
	{
		for (int i=0;i<_numIK;i++)
		{
			if (_ikList[i].getInfo()->BoneLinkList)
				delete[] _ikList[i].getInfo()->BoneLinkList;
		}
		delete[] _ikList;
	}

}
void AMBoneController::sortBones()
{
	BoneInfo *cii;
	BoneInfo* cij;

	//assign the root bone as parent to all bones without a defined parent
	for (int i=0;i<_numBones;i++)
	{
		cii = _bonesList[i].getInfo();

		//if this bone is not the root bone and it has no parent
		//assign the root bone as its parent
		if ((&(_bonesList[i]) != _rootBone) && (cii->Parent == NULL))
				cii->Parent = _rootBone;

		//initialize this bone
		_bonesList[i].calcOffsetAndLocalTransform();
	}
	//assign a child to each bone
	for (int i=0;i<_numBones;i++)
	{
		cii = _bonesList[i].getInfo();

		//search through bone list again trying to find child bone
		for (int j=0;j<_numBones;j++)
		{
			cij = _bonesList[j].getInfo();
		
			//only bother checking this bone if it's not the root bone
			//and check for match
			if ((&(_bonesList[j]) != _rootBone) && (cij->Parent == &_bonesList[i]))
			{
				//finally assign the child bone
				cii->FirstChild = &(_bonesList[j]);
				break; //found it, so exit search loop
			}
		}
	}
	//assign a sibling to each bone
	for (int i=0;i<_numBones;i++)
	{
		cii = _bonesList[i].getInfo();

		//make sure this bone can't be assigned as a sibling again
		_bonesList[i].setisTakenSibling(true);

		//search for a sibling bone
		for (int j=0;j<_numBones;j++)
		{
			cij = _bonesList[j].getInfo();

			//bone will be sibling if it's not the same bone and they have the same parent
			if ((cii->Parent == cij->Parent) && &(_bonesList[i]) != &(_bonesList[j]))
			{
				//make sure this bone isn't already assigned as a sibling
				if (!_bonesList[j].getisTakenSibling())
				{
					//finally assign the sibling
					cii->FirstSibling = &(_bonesList[j]);
					//make sure this bone also can't be assigned as a sibling again
					_bonesList[j].setisTakenSibling(true);
					break; //found it, so exit search loop
				}
			}
		}
	}
}
void AMBoneController::updateTransform(AMBone *bone, bool forceRepos)
{
	/*
	Using an iterative algorithm implemented with a stack instead of
	a recursive algorithm because some bone structures will cause overflows
	with resursion
	*/

	/*The snapshot structure with data used each iteration*/
	struct Snapshot
	{
		AMBone *bone;
		bool forceRepos;

		int stage;
	} curSnapshot;
	/*The snapshot stack for the algorithm*/
	std::stack<Snapshot> snapshotStack;

	//initialize all local variables needed for the algorithm
	curSnapshot.bone = bone;
	curSnapshot.forceRepos = forceRepos;
	curSnapshot.stage = 0;
	snapshotStack.push(curSnapshot);

	BoneInfo *ci;
	//loop until the stack is empty
	while (!snapshotStack.empty())
	{
		//pop the current snapshot off the top of the stack
		curSnapshot = snapshotStack.top();
		snapshotStack.pop();

		ci = curSnapshot.bone->getInfo();

		switch (curSnapshot.stage)
		{
		case 0:
			//prep the stack for stage 1 (first sibling check) when the next loop iteration hits
			curSnapshot.stage = 1;
			snapshotStack.push(curSnapshot);

			//do the calculations on the current bone
			curSnapshot.bone->calcCombinedTrans(curSnapshot.forceRepos);
			continue;
		case 1:
			curSnapshot.stage = 2; //when the recursive calling returns, we want to move on to the next stage
			snapshotStack.push(curSnapshot); //push the new snapshot on top of the stack so it is queued to be processed

			//check if need to process sibling
			if (ci->FirstSibling && !ci->FirstSibling->processed)
			{
				curSnapshot.bone = ci->FirstSibling; //change the bone to the one we will start processing
				curSnapshot.stage = 0; //start at stage 0 (operations on current bone)
				//parent matrix doesn't change because bones have same parent
				snapshotStack.push(curSnapshot); //drop the new snapshot on top of the stack so 
												 // next loop we begin recursive processing of the sibling bone
				continue; //skip right to next iteration
			}
			continue;
		case 2:
			//check if need to process child
			if (ci->FirstChild && !ci->FirstChild->processed)
			{
				//set up next iteration to process the child
				curSnapshot.bone = ci->FirstChild;
				curSnapshot.stage = 0; //start at stage 0 (operations on current bone)
				snapshotStack.push(curSnapshot);
				continue; //skip right to next iteration
			}
			continue;
		}
	}
	//reset state of each bone for next time skeleton is updated
	for (int i=0;i<_numBones;i++)
	{
		_bonesList[i].processed = false;
	}
}
void AMBoneController::setupBoneList(AMBone *b, unsigned long numBone)
{
	//assign bone count and list
	_numBones = (int)numBone;
	_bonesList = b;

	BoneInfo *ci;
	//search for center and root bone
	for (int i=0;i<_numBones;i++)
	{
		ci = _bonesList[i].getInfo();

		//find bones based on name match
		if (!strcmp(ci->Name, PMDMODEL_CENTERBONENAME) && _centerBone == NULL)
			_centerBone = &(_bonesList[i]);
		else if (!strcmp(ci->Name, PMDMODEL_ROOTBONENAME) && _rootBone == NULL)
			_rootBone = &(_bonesList[i]);
	}

	//if no defined center bone, use the first one in list
	if (!_centerBone && numBone >= 1)
		_centerBone = &(_bonesList[0]);

	//if no root bone, create a fake one
	if (!_rootBone && numBone >= 1)
	{
		_rootBone = new AMBone();
		_rootBone->getInfo()->Name = new char[strlen(PMDMODEL_ROOTBONENAME)+1];
		strcpy_s(_rootBone->getInfo()->Name, strlen(PMDMODEL_ROOTBONENAME)+1, PMDMODEL_ROOTBONENAME);
		_rootBone->getInfo()->FirstChild = _centerBone;
		_rootBone->init();

		generatedRoot = true;
	}

	//initialize all bones in the bone structure
	sortBones();
}
void AMBoneController::setupIKList(IK* ik, int numIK)
{
	//assign ik count and list
	_ikList = ik;
	_numIK = numIK;

	//initialize all IK chains
	for (int i=0;i<numIK;i++)
	{ 
		_ikList[i].init();
	}
}
void AMBoneController::solveIKs()
{
	for (int i=0;i<_numIK;i++)
	{
		_ikList[i].solve();
	}
}
void AMBoneController::updateTransformsFromRoot(bool forceRepos)
{
	updateTransform(_rootBone, forceRepos);
}

void AMBoneController::updateBonesFromMotion(AM_Motion *motion, bool isDefPos)
{
	BoneInfo* ci;
	for (int i=0;i<_numBones;i++)
	{
		ci = _bonesList[i].getInfo();

		//initialize position and rotation
		D3DXVECTOR3 newPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXQUATERNION newRot = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);

		//get either bind pose or current pose
		if (isDefPos)
			motion->getDefPose(ci->Name, i, &newPos, &newRot); //get default if setting def
		else
			motion->getPosRotAtTimeFromBone(ci->Name, i, &newPos, &newRot); //get pos and rot

		//calculate the rotation and translation in bone using new values
		_bonesList[i].calcTransFromPosAndRot(newPos, newRot);
	}
}

AMBone* AMBoneController::getBoneList()
{
	return _bonesList;
}
AMBone* AMBoneController::getCenterBone()
{
	return _centerBone;
}