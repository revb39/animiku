/************************************************************************
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
*																		*
* AniMiku is Copyright (c) 2014 Justin Byers							*
*************************************************************************/

#include "Common.fx"

matrix lightViewProjMatrix;


SamplerComparisonState CmpShadowSampler
{
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;

	ComparisonFunc = LESS_EQUAL;
};

struct SHADOW_PS_INPUT
{
	float4 pos : SV_POSITION;
};
SHADOW_PS_INPUT vs_Shadow(VS_INPUT i)
{
	SHADOW_PS_INPUT o;

	float4 p = skinVert(i.position, i.boneLinks, i.boneWeights);

	o.pos = mul(p, mul(worldMatrix, lightViewProjMatrix));
	return o;
}
void ps_Shadow(SHADOW_PS_INPUT input)
{
	
}

float2 texOffset(int u, int v)
{
	return float2(u * 1.0f/1280.0f, v * 1.0f/800.0f);
}
float getShadowFactor(float4 lpos)
{
	//shadow mapping stuff
	lpos.xyz /= lpos.w;

	lpos.x = lpos.x/2 + 0.5;
	lpos.y = lpos.y/-2 + 0.5;
	lpos.z -= 0.005f;

	float sum = 0;
	float x,y;
	for (y=-1.5;y<=1.5;y+= 1.0)
	{
		for (x=-1.5;x<=1.5;x+=1.0)
		{
			sum += shadowMap.SampleCmpLevelZero(CmpShadowSampler, lpos.xy + texOffset(x,y), lpos.z);
		}
	}
	return sum / 16.0;
}