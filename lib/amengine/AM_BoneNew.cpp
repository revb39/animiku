/************************************************************************
*	AM_BoneNew.cpp -- Bone definition used in character animation		*
*					  Copyright (c) 2014 Justin Byers					*
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

#include "AM_BoneNew.h"

AMBone::AMBone()
{
	//initialize local variables
	ZeroMemory(&_info, sizeof(BoneInfo));
	isSimulated = false;
	isTakenSibling = false;
	processed = false;
	//initialize matrices
	D3DXMatrixIdentity(&combTrans);
	D3DXMatrixIdentity(&finalRot);
	D3DXMatrixIdentity(&finalPos);
	D3DXMatrixIdentity(&skinningMat);
	D3DXMatrixIdentity(&localOffsetTrans);
	D3DXMatrixIdentity(&localOffsetTransInv);
	D3DXMatrixIdentity(&skinningOffsetTranslation);
	//initialize vectors
	combRot = D3DXQUATERNION(0,0,0,1);
	offset = D3DXVECTOR3(0,0,0);
	trans = D3DXVECTOR3(0,0,0);
}
AMBone::~AMBone()
{
	if (_info.Name)
		delete[] _info.Name;
	if (_info.NameEnglish)
		delete[] _info.NameEnglish;
}

BoneInfo* AMBone::getInfo()
{
	return &_info;
}
void AMBone::init()
{
	//initialize the combined transformation matrix to initial position
	D3DXMatrixTranslation(&combTrans, _info.InitialPosition.x, _info.InitialPosition.y, _info.InitialPosition.z);
	//set the bind pose combined transformation matrix
	D3DXMatrixTranslation(&defCombTrans, _info.InitialPosition.x, _info.InitialPosition.y, _info.InitialPosition.z);
	//get the inverse bind post combined transformation matrix
	D3DXMatrixInverse(&defCombTransInv, NULL, &defCombTrans);
	//calculate the initial skinning matrix
	skinningMat = defCombTransInv * defCombTrans;
}

void AMBone::setisTakenSibling(bool t)
{
	isTakenSibling = t;
}
bool AMBone::getisTakenSibling()
{
	return isTakenSibling;
}


void AMBone::calcOffsetAndLocalTransform()
{
	if (_info.Parent)
	{
		//calculate the vector offset from parent bone
		offset = _info.InitialPosition - D3DXVECTOR3(_info.Parent->combTrans(3, 0),
													 _info.Parent->combTrans(3, 1),
													 _info.Parent->combTrans(3, 2));

		//calculate the local offset matrix and inverse local offset matrix
		D3DXMatrixTranslation(&localOffsetTrans, offset.x, offset.y, offset.z);
		D3DXMatrixInverse(&localOffsetTransInv, NULL, &localOffsetTrans);
	}
	else
	{
		//if the bone has no parent, the offset will be relative to the world
		//just use the combined transformation matrix
		localOffsetTrans = combTrans;
		D3DXMatrixInverse(&localOffsetTransInv, NULL, &localOffsetTrans);
	}
}

void AMBone::calcTransFromPosAndRot(float pos[3], float rot[4])
{
	//only set the translation if the bone can move
	//otherwise set local translation to 0
	if (_info.TransConfig.CanMove)
		trans = pos;
	else
		trans = D3DXVECTOR3(0,0,0);

	//only set the rotation if the bone can rotate
	//otherwise set rotation to 0
	if (_info.TransConfig.CanRotate)
	{
		//if fixed axis, calculate rotation around defined axis
		if (_info.TransConfig.FixedAxis)
		{
			float angle;
			D3DXVECTOR3 oldAxis; //throw this away
			//compute the current rotation angle
			D3DXQuaternionToAxisAngle(&D3DXQUATERNION(rot[0], rot[1], rot[2], rot[3]), &oldAxis, &angle);
			//create a new quaternion using the defined fixed axis and old rotation angle
			D3DXQuaternionRotationAxis(&combRot, &_info.TransVals.FixedAxis, angle);
		}
		else
		{
			//otherwise, just set the rotation
			combRot = rot;
		}
	}
	else
	{
		combRot = D3DXQUATERNION(0,0,0,1);
	}
}
void AMBone::calcCombinedTrans(bool forceRepos)
{
	//after this call exits processing is complete
	processed = true;

	//if this bone is simulated and forceRepos is not set, don't do anything
	if (isSimulated && !forceRepos)
		return;

	//check if inheriting rotation from another bone
	if (_info.TransConfig.RotationInherit)
	{
		D3DXQUATERNION slerpRaw;
		//interpolate to inherited rotation by rot inhere rate
		D3DXQuaternionSlerp(&slerpRaw, &D3DXQUATERNION(0,0,0,1), &(_info.TransVals.BoneInheritRotation->getRot()), _info.TransVals.RotationInhereRate);
		//combine inherited rotation with this bone's rotation
		combRot = slerpRaw * combRot;
	}
	if (_info.TransConfig.TranslationInherit)
	{
		D3DXVECTOR3 lerpRaw;
		//interpolate to inherited translation by translation inhere rate
		D3DXVec3Lerp(&lerpRaw, &D3DXVECTOR3(0,0,0), &(_info.TransVals.BoneInheritTranslation->getPosLocal()), _info.TransVals.TranslationInhereRate);
		//combine inherited translation with this bone's translation
		trans = lerpRaw + trans;
	}

	//put the local rotation and translation into matrices for calculation
	D3DXMatrixRotationQuaternion(&finalRot, &combRot);
	D3DXMatrixTranslation(&finalPos, trans.x, trans.y, trans.z);

	//calculate combined transformation matrix from local rotation, translation, and offset matrices
	combTrans = (finalRot*finalPos)*localOffsetTrans;

	//if this bone has a parent, combine with the parent's combined transformation matrix
	if (_info.Parent)
		combTrans *= _info.Parent->getCombinedTrans();

	//calculate the skinning matrix for vertex blending
	calcSkinningTrans();
}

D3DXMATRIX AMBone::getCombinedTrans()
{
	return combTrans;
}
D3DXMATRIX AMBone::getSkinningTrans()
{
	return skinningMat;
}

D3DXQUATERNION AMBone::getRot()
{
	return combRot;
}
D3DXVECTOR3 AMBone::getPosLocal()
{
	return trans;
}
D3DXVECTOR3 AMBone::getPosWorld()
{
	return D3DXVECTOR3(combTrans(3,0), combTrans(3,1), combTrans(3,2));
}

void AMBone::setRot(D3DXQUATERNION rot)
{
	combRot = rot;
}
void AMBone::UpdateFromIK()
{
	//put the rotation into a matrix & assign to combined trans matrix
	D3DXMatrixRotationQuaternion(&finalRot, &combRot);
	combTrans = finalRot;

	//set combined translation to offset
	combTrans(3,0) = offset.x;
	combTrans(3,1) = offset.y;
	combTrans(3,2) = offset.z;

	//if this bone has a parent, combine with the parent's combined transformation matrix
	if (_info.Parent)
	{
		combTrans *= _info.Parent->getCombinedTrans();
	}

	//calculate the skinning matrix for vertex blending
	calcSkinningTrans();
}
void AMBone::calcSkinningTrans()
{	
	D3DXMATRIX skinningOffset;

	//skinning matrix = inverse bind pose * local rotation * offset from bind pose * parent matrix

	//calculate the matrix for the offset of this bone from the bind position
	D3DXVECTOR3 offsetPos = _info.InitialPosition+trans;
	D3DXMatrixTranslation(&skinningOffset, offsetPos.x, offsetPos.y, offsetPos.z);

	//calculate the skinning matrix
	skinningMat = defCombTransInv * finalRot;
	D3DXMatrixMultiply(&skinningMat, &skinningMat, &skinningOffset);

	//if this bone has a parent, combine with the parent's skinning matrix
	if (_info.Parent)
		skinningMat *= _info.Parent->getSkinningTrans();
}
void AMBone::markSimulated(bool s)
{
	isSimulated = s;
}

void AMBone::setCombTrans(D3DXMATRIX mat)
{
	//put rotation into its matrix
	D3DXMatrixRotationQuaternion(&finalRot, &combRot);

	//convert combined trans mat to offset from bind pose
	mat *= defCombTransInv;

	//calculate the skinning matrix for vertex blending
	D3DXVECTOR3 bindPos = D3DXVECTOR3(defCombTrans(3,0), defCombTrans(3,1), defCombTrans(3,2));
	D3DXVECTOR3 newPos = D3DXVECTOR3(mat(3,0), mat(3,1), mat(3,2));
	D3DXVECTOR3 offsetPos = bindPos+newPos;
	D3DXMatrixTranslation(&skinningOffsetTranslation, offsetPos.x, offsetPos.y, offsetPos.z);
	skinningMat = defCombTransInv * finalRot * skinningOffsetTranslation;

	//set the combined transformation matrix
	memcpy(&combTrans, &mat, sizeof(D3DXMATRIX));
}
bool AMBone::getIsSimulated()
{
	return isSimulated;
}