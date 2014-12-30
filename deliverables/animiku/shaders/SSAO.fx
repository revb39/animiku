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

#ifndef SSAO_FX_
#define SSAO_FX_

#include "Common.fx"

float ssaoRad;
float ssaoIntensity;
float ssaoScale;
float ssaoBias;
float samples;

float3 getPosition(float2 uv)
{
	return posMap.Sample(DifferredSampler, uv).xyz;
}
float3 getNormal(float2 uv)
{
	return normalize(normalMap.Sample(DifferredSampler, uv).xyz * 2.0f - 1.0f);
}
float getDepth(float2 uv)
{
	return depthMap.Sample(DifferredSampler, uv).w;
}
float2 getRandom(float2 uv)
{
	return normalize(randomTexture.Sample(RandomSampler, texelSize*uv/float2(64,64)).xy * 2.0f - 1.0f);
}
float calcOcclusion(float2 texcoord, float2 uv, float3 p, float3 n)
{
	float3 diff = getPosition(texcoord + uv) - p;
	const float3 v = normalize(diff);
	const float d = length(diff)*ssaoScale;
	return max(0.0,dot(n,v)-ssaoBias)*(1.0/(1.0+d))*ssaoIntensity;
}
float getOcclusion(float2 uv)
{
	const float2 vec[4] = {float2(1,0),float2(-1,0),
				float2(0,1),float2(0,-1)};

	float3 p = getPosition(uv);
	float3 n = getNormal(uv);
	float2 rand = getRandom(uv);

	float ao = 0.0f;
	float rad = ssaoRad/p.z;

	
	float i = 1/(samples/4.0f);
	for (int j = 0; j < 4; ++j)
	{
		float2 coord1 = reflect(vec[j],rand)*rad;
		float2 coord2 = float2(coord1.x*0.707 - coord1.y*0.707,
					coord1.x*0.707 + coord1.y*0.707);
		
		for (int s=1;s<=(int)(samples/2.0f);s+=2)
		{
			ao += calcOcclusion(uv, coord1*(s*i), p, n);
			ao += calcOcclusion(uv, coord2*((s+1)*i), p, n);
		}
	}
	ao/=samples;


	return 1.0-ao;
}

#endif