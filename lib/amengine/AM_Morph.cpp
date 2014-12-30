/************************************************************************
*	AM_Morph.cpp -- Morph management and calculation					*
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

#include "AM_Morph.h"

AMMorph::AMMorph()
{
	ZeroMemory(&_info, sizeof(MorphInfo));
	_groupedScl = 1.0f;
}
AMMorph::~AMMorph()
{
	if (_info.Name)
		delete[] _info.Name;
	if (_info.NameEnglish)
		delete[] _info.NameEnglish;
	if (_info.Groups)
		delete[] _info.Groups;
	if (_info.Vertices)
		delete[] _info.Vertices;
	if (_info.UVs)
		delete[] _info.UVs;
	if (_info.Bones)
		delete[] _info.Materials;
}

MorphInfo* AMMorph::getInfo()
{
	return &_info;
}

void AMMorph::calculate(float weight)
{
	//0 has no effect, >1 means an error occurred, so don't do any calculations
	if (weight == 0 || weight > 1)
		return;

	//multiply by scalar value to add effect from group morphs
	weight *= _groupedScl;
	
	for (int i=0;i<_info.OffsetCount;i++)
	{
		if (_info.Type == MorphDefs::Group)
		{
			//I'm going to assume this is what the groups do
			//re-calculate the grouped morph using weight=(morph rate)*(weight passed to this function)
			_info.Groups[i].GroupedMorph->groupScl(_info.Groups[i].MorphRate*weight);
		}
		else if (_info.Type == MorphDefs::Vertex)
		{
			//calculate the offset based on max offset and weight
			D3DXVECTOR3 weighted = _info.Vertices[i].MaxOffset*weight;
			_info.Vertices[i].Vertex->curFaceTrans(3,0) += weighted.x;
			_info.Vertices[i].Vertex->curFaceTrans(3,1) += weighted.y;
			_info.Vertices[i].Vertex->curFaceTrans(3,2) += weighted.z;
			//calculate the final position by translating based on weighted offset
			D3DXVECTOR4 tv, tvn;
			D3DXVec3Transform(&tv, &_info.Vertices[i].Vertex->origPos, &_info.Vertices[i].Vertex->curFaceTrans);
			D3DXVec3Transform(&tvn, &_info.Vertices[i].Vertex->origNormal, &_info.Vertices[i].Vertex->curFaceTrans);

			//store the scaled position and normal back in the vertex
			_info.Vertices[i].Vertex->pos = (D3DXVECTOR3)tv;
			_info.Vertices[i].Vertex->normal = (D3DXVECTOR3)tvn;
		}
		else if (_info.Type == MorphDefs::UV) //not using add UV's yet
		{
			//calculate the offset based on max offset and weight
			D3DXVECTOR2 weighted = _info.UVs[i].MaxOffset*weight;
			_info.UVs[i].Vertex->curFaceTransUV(3,0) += weighted.x;
			_info.UVs[i].Vertex->curFaceTransUV(3,1) += weighted.y;
			//calculate the final UV by translating based on weighted offset
			D3DXVECTOR4 tv;
			D3DXVec2Transform(&tv, &_info.UVs[i].Vertex->origUV, &_info.UVs[i].Vertex->curFaceTransUV);

			//store the scaled UV back in the vertex
			_info.UVs[i].Vertex->uv = (D3DXVECTOR2)tv;
		}
		else if (_info.Type == MorphDefs::Bone)
		{
			//calculate the translation offset based on max offset and weight
			D3DXVECTOR3 weightedT = _info.Bones[i].MaxOffset*weight;
			//calculate the rotation offset based on max rotation and weight
			D3DXQUATERNION weightedR;
			D3DXQuaternionSlerp(&weightedR, &D3DXQUATERNION(0,0,0,1), &_info.Bones[i].MaxRotation, weight);
			//calculate the final rotation
			weightedR = _info.Bones[i].Bone->getRot() * weightedR;
			//calculate the bone's world transform
			_info.Bones[i].Bone->calcTransFromPosAndRot(weightedT, weightedR);
		}
	}
	
	_groupedScl = 1; //reset this to keep it consistent on the next frame
}

void AMMorph::groupScl(float scl)
{
	_groupedScl *= scl;
}

void AMMorph::clearVertTransforms()
{
	if (_info.Type == MorphDefs::Vertex || _info.Type == MorphDefs::UV)
	{
		for (int i=0;i<_info.OffsetCount;i++)
		{
			if (_info.Type == MorphDefs::Vertex)
				D3DXMatrixIdentity(&_info.Vertices[i].Vertex->curFaceTrans);
			else
				D3DXMatrixIdentity(&_info.UVs[i].Vertex->curFaceTransUV);
		}
	}
}