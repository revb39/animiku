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
#include "SSAO.fx"
#include "Blur.fx"

//vertex shader output
struct DIF_VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 actPos : TEXCOORD0;
	float3 normal : TEXCOORD1;
};

//pixel shader output to multiple render targets
struct DIF_PS_OUTPUT
{
	float4 posMap : SV_Target0;
	float4 normalMap : SV_Target1;
	float4 depthMap : SV_Target2;
};

//vertex shader for normal/pos/depth/occlusion map generation
DIF_VS_OUTPUT VS_Dif(VS_INPUT i)
{
	DIF_VS_OUTPUT o;

	//skin the vertex to the bones on the model
	float4 p = skinVert(i.position, i.boneLinks, i.boneWeights);
	//skin the normal to the bones on the model
	float3 n = skinNorm(i.normal, i.boneLinks, i.boneWeights);

	//move the position to world space
	float4 worldPos = mul(p, worldMatrix);

	//move the position to screen space for pixel output
	o.pos = mul(worldPos, viewMatrix);
	o.pos = mul(o.pos, projMatrix);

	//move the position to view space for the position map
	o.actPos = mul(worldPos, viewMatrix);

	//move the normal to view space for the normal map
	o.normal = mul(n, worldMatrix);
	o.normal = normalize(mul(o.normal, viewMatrix));

	return o;
}

//pixel shader for normal/pos/depth map generation
DIF_PS_OUTPUT PS_Dif(DIF_VS_OUTPUT i)
{
	DIF_PS_OUTPUT o;
	o.posMap = float4(i.actPos, 1);
	o.normalMap = float4(0.5*i.normal+0.5, 1);
	o.depthMap = float4(i.actPos.z, 0, 0, 1);

	return o;
}

//pixel shader for occlusion map generation
float4 PS_DifOcc(DIF_VS_OUTPUT i) : SV_Target
{
	return getOcclusion((i.pos.xy)/texelSize);
}

//pixel shader for blur stage 1
float4 PS_DifBlur(DIF_VS_OUTPUT i) : SV_Target
{
	return float4(blurHor(occlusionMap, (i.pos.xy)/texelSize, 1/texelSize), 1);
}