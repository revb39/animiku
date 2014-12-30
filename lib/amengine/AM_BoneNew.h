/************************************************************************
*	AM_BoneNew.h -- Bone definition used in character animation			*
*					Copyright (c) 2014 Justin Byers						*
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

#ifndef _BONE_H_
#define _BONE_H_

#include "AMTypes.h"
#include "btBulletDynamicsCommon.h"

/*Name of knee bone used in PMD (defined by MMD spec)*/
#define PMDMODEL_KNEENAME "‚Ð‚´"

/*Declare the class here for datastructures below*/
class AMBone;

/*Bone transformation configuration (defined by model)*/
struct BoneTransConfig
{
	bool CanRotate;
	bool CanMove;
	bool IK;
	bool RotationInherit;
	bool TranslationInherit;
	bool FixedAxis;
	bool LocalAxes;
	bool AfterPhysicalDeformation;
	bool ExternalParentDeformation;
};

/*Values used in bone transformation (defined by model)*/
struct BoneTransVals
{
	/*Inherent rotation from another bone*/
	AMBone* BoneInheritRotation;
	/*Rate at which to inherit the rotation*/
	float RotationInhereRate;
	//Inherent translation from another bone
	AMBone* BoneInheritTranslation;
	/*Rate at which to inherit the translation*/
	float TranslationInhereRate;
	/*Fixed axis to rotate around*/
	D3DXVECTOR3 FixedAxis;
	/*The hell do these do lol*/
	D3DXVECTOR3 LocalAxisX;
	D3DXVECTOR3 LocalAxisZ;
	float ExternalParentDeformationKey;
};

/*Other information about this bone (defined by model)*/
struct BoneInfo
{
	/*Name in Japanese and English*/
	char* Name;
	char* NameEnglish;
	/*Initial position of the bone*/
	D3DXVECTOR3 InitialPosition;
	/*The transformation config*/
	BoneTransConfig TransConfig;
	/*The transformation values*/
	BoneTransVals TransVals;
	/*Index of parent bone*/
	int ParentIndex;
	/*Related bones*/
	AMBone* Parent;
	AMBone* FirstChild;
	AMBone* FirstSibling;
	/*Either the target bone, or the bone rotation and/or translation are inherited from*/
	AMBone* InhereBone;

};
class AMBone
{
private:
	/*The bone info structure*/
	BoneInfo _info;

	/*This bone is controlled by physics simulation*/
	bool isSimulated;

	/*During bone structure building, set true once assigned as a sibling to another bone*/
	bool isTakenSibling;

	/*World transformation matrices*/
	D3DXMATRIX combTrans, finalRot, finalPos;
	
	/*Offset transformation matrices, used to calculate child bone positions*/
	D3DXMATRIX localOffsetTrans, localOffsetTransInv;

	/*Bind pose transformation matrices*/
	D3DXMATRIX defCombTrans, defCombTransInv;

	/*Matrices used in vertex blending*/
	D3DXMATRIX skinningMat, skinningOffsetTranslation;

	/*Set by motion data and other factors - LOCAL*/
	D3DXVECTOR3 trans;
	D3DXQUATERNION combRot;

	/*Offset from parent bone world position*/
	D3DXVECTOR3 offset;

	/*
		void calcSkinningTrans()

		Calculate the skinning transformation matrix used when vertex blending

		Parameters:		None
		Returns:		None
	*/
	void calcSkinningTrans();
public:
	AMBone();
	~AMBone();

	/*Set true during bone structure solving one this bone is solved*/
	bool processed;

	/*
		BoneInfo* getInfo()

		Gets pointer to the info structure to set up bone

		Returns:		Pointer to the info structure
	*/
	BoneInfo* getInfo();

	/*
		void init()

		Initialize this bone. Must be called after setting info structure
		and before using this bone
	*/
	void init();

	/*
		void setisTakenSibling()

		Set if this bone is already assigned as a sibling

		Parameters:		t		boolean value to set
	*/
	void setisTakenSibling(bool t);
	/*
		bool getIsTakenSibling()

		Get if this bone is already assigned as a sibling

		Returns:		boolean value true/false
	*/
	bool getisTakenSibling();

	//math and transforms/matrix calculations
	/*
		void calcOffsetAndLocalTransform()

		Calculate the offset from parent as local transform
	*/
	void calcOffsetAndLocalTransform();
	/*
		void calcTransFromPosAndRot()

		Calculate translation & rotation from provided position and rotation offsets

		Parameters:		pos		Position relative to bind pos
						rot		Rotation relative to bind rot
	*/
	void calcTransFromPosAndRot(float pos[3], float rot[4]);
	/*
		D3DXMATRIX getCombinedTrans()

		Get the combined (world) transformation matrix of this bone
	*/
	D3DXMATRIX getCombinedTrans();
	/*
		D3DXMATRIX getSkinningTrans()

		Get the skinning transformation matrix of this bone
	*/
	D3DXMATRIX getSkinningTrans();
	/*
		D3DXQUATERNION getRot()

		Get the rotation of this bone
	*/
	D3DXQUATERNION getRot();
	/*
		D3DXVECTOR3 getPosLocal()

		Get the local position relative to bind pos
	*/
	D3DXVECTOR3 getPosLocal();
	/*
		D3DXVECTOR3 getPosWorld()

		Get the combined (world) position of this bone
	*/
	D3DXVECTOR3 getPosWorld();

	/*
		void setCombTrans()

		Set the combined (world) transformation matrix directly.
		Currently only used by physics engine for simulated bones.
		Requires setRot() to be called first.

		Parameters:		mat		matrix to set as combined matrix
	*/
	void setCombTrans(D3DXMATRIX mat);

	/*
		void setRot()

		Set the rotation of the bone directly.
		Currently only used by physics engine and IK solver

		Parameters:		rot		rotation to set
	*/
	void setRot(D3DXQUATERNION rot);
	/*
		void UpdateFromIK()

		Update & finalize combined transformation matrix using only rotation.
		Used only by IK solver.
	*/
	void UpdateFromIK();
	/*
		void calcCombinedTrans()

		Finalize & update the combined (world) transformation matrix 

		Parameters:		forceRepos		Always force calculation/ignore simulation flag
	*/
	void calcCombinedTrans(bool forceRepos);

	/*
		void markSimulated()

		Mark this bone as simulated

		Parameters:		s		true/false
	*/
	void markSimulated(bool s);
	/*
		bool getIsSimulated()

		Get if this bone is simulated
	*/
	bool getIsSimulated();
};
#endif