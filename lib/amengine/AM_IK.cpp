/************************************************************************
*	AM_IK.cpp -- IK chain solver										*
*				 Copyright (c) 2014 Justin Byers						*
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

#include "AM_IK.h"

IK::IK()
{
	ZeroMemory(&_info, sizeof(IKInfo));
	simulated = false;
}
IK::~IK()
{	
	ZeroMemory(&_info, sizeof(IKInfo));
}
bool IK::checkSimulated()
{
	//check the destination (target) bone and end effector bone first
	if (_info.DestinationBone->getIsSimulated() || _info.EndEffectorBone->getIsSimulated())
		return true;
	//next check the other links/bones in the chain
	if (_info.NumberOfLinks)
	{
		for (int i=0;i<(int)_info.NumberOfLinks;i++)
		{
			if (_info.BoneLinkList[i].LinkBone->getIsSimulated())
				return true;
		}
	}
	return false;
}

IKInfo* IK::getInfo()
{
	return &_info;
}
void IK::init()
{
	//initializing here just means setting quaternions to initial values
	if (_info.NumberOfLinks)
	{
		for (int i=0;i<(int)_info.NumberOfLinks;i++)
		{
			_info.BoneLinkList[i].IKQuat = D3DXQUATERNION(0,0,0,1);
		}
	}
}

float GetUpperLowerRadian(float fR, float lower, float upper, bool loopFlag)
{
	if (fR < lower)
	{
		float num = 2.0f * lower - fR;
		if (num <= upper && loopFlag)
			fR = num;
		else
			fR = lower;
	}
	if (fR > upper)
	{
		float num2 = 2.0f * upper - fR;
		if (num2 >= lower && loopFlag)
			fR = num2;
		else
			fR = upper;
	}
	return fR;
}
void IK::solve()
{
	//don't solve anything if this chain is simulated
	if (checkSimulated())
		return;

	//set up local variables used in solving the IK chain
	AMBone* destBone = _info.DestinationBone;
	AMBone* endEffBone = _info.EndEffectorBone;
	AMBone* curBone = NULL;
	D3DXVECTOR3 dest = D3DXVECTOR3(destBone->getCombinedTrans()(3,0), destBone->getCombinedTrans()(3,1), destBone->getCombinedTrans()(3,2));
	
	//lol I still have no clue how this crazy shit works
	//algorithm is pretty much taken directly from MMD
	IKLink *cl;
	BoneInfo *ci;
	for (int i=0;i<(int)_info.IterationCount;i++)
	{
		for (int b=0;b<(int)_info.NumberOfLinks;b++)
		{
			cl = &_info.BoneLinkList[b];
			curBone = cl->LinkBone;
			ci = curBone->getInfo();

			D3DXVECTOR3 left = curBone->getPosWorld()-endEffBone->getPosWorld();
			D3DXVECTOR3 right = curBone->getPosWorld()-destBone->getPosWorld();
			D3DXVECTOR3 axis;

			D3DXVECTOR3 vec = left - right;
			if (D3DXVec3Length(&vec)*D3DXVec3Length(&vec) < 1E-04f)
			{
				i = _info.IterationCount;
				break;
			}
			D3DXVec3Normalize(&left, &left);
			D3DXVec3Normalize(&right, &right);

			D3DXVec3Cross(&axis, &left, &right);
			if (D3DXVec3Length(&axis)*D3DXVec3Length(&axis) < 1E-7 && i > 0)
				continue;
			D3DXVec3Normalize(&axis, &axis);

			/*if (cl->AngleLimit && i < _info.IterationCount/2)
			{
				if (cl->LowerLimit.y == 0.0f && cl->UpperLimit.y == 0.0f && cl->LowerLimit.z == 0.0f && cl->UpperLimit.z == 0.0f)
				{
					float n = (axis.x*ci->Parent->getCombinedTrans()._11) + (axis.y*ci->Parent->getCombinedTrans()._12) + (axis.z*ci->Parent->getCombinedTrans()._13);
					if (n >= 0.0f)
						axis.x = 1.0f;
					else
						axis.x = -1.0f;
					axis.y = 0.0f;
					axis.z = 0.0f;
				}
				else
				{
					if (cl->LowerLimit.x == 0.0f && cl->UpperLimit.x == 0.0f && cl->LowerLimit.z == 0.0f && cl->UpperLimit.z == 0.0f)
					{	
						float n = (axis.x*ci->Parent->getCombinedTrans()._21) + (axis.y*ci->Parent->getCombinedTrans()._22) + (axis.z*ci->Parent->getCombinedTrans()._23);
						if (n >= 0.0f)
							axis.y = 1.0f;
						else
							axis.y = -1.0f;
						axis.x = 0.0f;
						axis.z = 0.0f;
					}
					else
					{
						if (cl->LowerLimit.x == 0.0f && cl->UpperLimit.x == 0.0f && cl->LowerLimit.y == 0.0f && cl->UpperLimit.y == 0.0f)
						{
							float n = (axis.x*ci->Parent->getCombinedTrans()._31) + (axis.y*ci->Parent->getCombinedTrans()._32) + (axis.z*ci->Parent->getCombinedTrans()._33);
							if (n >= 0.0f)
								axis.z = 1.0f;
							else
								axis.z = -1.0f;
							axis.x = 0.0f;
							axis.y = 0.0f;
						}
						else
						{
							D3DXVECTOR3 v;
							v.x = (axis.x*curBone->getCombinedTrans()._11)+(axis.y*curBone->getCombinedTrans()._12)+(axis.z*curBone->getCombinedTrans()._13);
							v.y = (axis.x*curBone->getCombinedTrans()._21)+(axis.y*curBone->getCombinedTrans()._22)+(axis.z*curBone->getCombinedTrans()._23);
							v.z = (axis.x*curBone->getCombinedTrans()._31)+(axis.y*curBone->getCombinedTrans()._32)+(axis.z*curBone->getCombinedTrans()._33);
							D3DXVec3Normalize(&axis, &v);
						}
					}
				}
			}
			else*/
			{
				D3DXVECTOR3 v;
				v.x = (axis.x*curBone->getCombinedTrans()._11)+(axis.y*curBone->getCombinedTrans()._12)+(axis.z*curBone->getCombinedTrans()._13);
				v.y = (axis.x*curBone->getCombinedTrans()._21)+(axis.y*curBone->getCombinedTrans()._22)+(axis.z*curBone->getCombinedTrans()._23);
				v.z = (axis.x*curBone->getCombinedTrans()._31)+(axis.y*curBone->getCombinedTrans()._32)+(axis.z*curBone->getCombinedTrans()._33);
				D3DXVec3Normalize(&axis, &v);
			}
			float dot = D3DXVec3Dot(&left, &right);
			if (dot > 1.0f)
				dot = 1.0f;
			else if (dot < -1.0f)
				dot = -1.0f;
			float angle = acosf(dot);
			if (angle > (_info.AngleConstraint * (b+1)*2))
			{
				angle = (_info.AngleConstraint * (b+1)*2);
			}
			D3DXQUATERNION newIkRot;
			D3DXQuaternionRotationAxis(&newIkRot, &axis, angle);
			D3DXQuaternionMultiply(&cl->IKQuat, &newIkRot, &cl->IKQuat);
			if (i == 0) //on first loop, apply rotation taken from the motion data
				D3DXQuaternionMultiply(&cl->IKQuat, &cl->IKQuat, &curBone->getRot());

			D3DXMATRIX matrix;
			D3DXMatrixRotationQuaternion(&matrix, &cl->IKQuat);
			if (cl->AngleLimit)
			{
				if ((double)cl->LowerLimit.x > -1.5707963267948966 && (double)cl->UpperLimit.x < 1.5707963267948966)
				{
					float num5 = -matrix._32;
					float num6 = asinf(num5);
					if (abs(num6) > 1.535889f)
					{
						if (num6 < 0.0f)
							num6 = -1.535889f;
						else
							num6 = 1.535889f;
					}
					float num7 = cosf(num6);
					float num8 = matrix._31/num7;
					float num9 = matrix._33/num7;
					float num10 = atan2f(num8, num9);
					float num11 = matrix._12/num7;
					float num12 = matrix._22/num7;
					float num13 = atan2f(num11, num12);
					bool loopFlag = i<(int)_info.IterationCount/2;
					num6 = GetUpperLowerRadian(num6, cl->LowerLimit.x, cl->UpperLimit.x, loopFlag);
					num10 = GetUpperLowerRadian(num10, cl->LowerLimit.y, cl->UpperLimit.y, loopFlag);
					num13 = GetUpperLowerRadian(num13, cl->LowerLimit.z, cl->UpperLimit.z, loopFlag);
					D3DXMATRIX nx, ny, nz;
					D3DXMatrixRotationX(&nx, num6);
					D3DXMatrixRotationY(&ny, num10);
					D3DXMatrixRotationZ(&nz, num13);
					matrix = nz*nx*ny;
				}
				else
				{
					if ((double)cl->LowerLimit.y > -1.5707963267948966 && (double)cl->UpperLimit.y < 1.5707963267948966)
					{
						float num14 = -matrix._13;
						float num15 = asinf(num14);
						if (abs(num15) > 1.535889f)
						{
							if (num15 < 0.0f)
							{
								num15 = -1.535889f;
							}
							else
							{
								num15 = 1.535889f;
							}
						}
						float num16 = cosf(num15);
						float num17 = matrix._23 / num16;
						float num18 = matrix._33 / num16;
						float num19 = atan2f(num17, num18);
						float num20 = matrix._12 / num16;
						float num21 = matrix._11 / num16;
						float num22 = atan2f(num20, num21);
						bool loopFlag = i<(int)_info.IterationCount/2;
						num19 = GetUpperLowerRadian(num19, cl->LowerLimit.x, cl->UpperLimit.x, loopFlag);
						num15 = GetUpperLowerRadian(num15, cl->LowerLimit.y, cl->UpperLimit.y, loopFlag);
						num22 = GetUpperLowerRadian(num22, cl->LowerLimit.z, cl->UpperLimit.z, loopFlag);
						D3DXMATRIX nx, ny, nz;
						D3DXMatrixRotationX(&nx, num19);
						D3DXMatrixRotationY(&ny, num15);
						D3DXMatrixRotationZ(&nz, num22);
						matrix = nx*ny*nz;
					}
					else
					{
						float num23 = -matrix._21;
						float num24 = asinf(num23);
						if (abs(num24) > 1.535889f)
						{
							if (num24 < 0.0f)
							{
								num24 = -1.535889f;
							}
							else
							{
								num24 = 1.535889f;
							}
						}
						float num25 = cosf(num24);
						float num26 = matrix._23 / num25;
						float num27 = matrix._22 / num25;
						float num28 = atan2f(num26, num27);
						float num29 = matrix._31 / num25;
						float num30 = matrix._11 / num25;
						float num31 = atan2f(num29, num30);
						bool loopFlag = i<(int)_info.IterationCount/2;
						num28 = GetUpperLowerRadian(num28, cl->LowerLimit.x, cl->UpperLimit.x, loopFlag);
						num31 = GetUpperLowerRadian(num31, cl->LowerLimit.y, cl->UpperLimit.y, loopFlag);
						num24 = GetUpperLowerRadian(num24, cl->LowerLimit.z, cl->UpperLimit.z, loopFlag);
						D3DXMATRIX nx, ny, nz;
						D3DXMatrixRotationX(&nx, num28);
						D3DXMatrixRotationY(&ny, num31);
						D3DXMatrixRotationZ(&nz, num24);
						matrix = ny*nz*nx;
					}
				}
				D3DXQuaternionRotationMatrix(&cl->IKQuat, &matrix);
			}
			curBone->setRot(cl->IKQuat);
			//curBone->setCombTrans(matrix);

			for (int j=b;j>=0;j--)
			{
				_info.BoneLinkList[j].LinkBone->UpdateFromIK();
			}
			endEffBone->UpdateFromIK();
		}
	}

	for (int i=0;i<(int)_info.NumberOfLinks;i++)
	{
		_info.BoneLinkList[i].IKQuat = D3DXQUATERNION(0,0,0,1);
	}
}