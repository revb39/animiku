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

#ifndef _COMMON_HLSL_
#define _COMMON_HLSL_
//collection of bone matrices
matrix MatrixPalette[255];
matrix worldMatrix;
matrix viewMatrix;
matrix projMatrix;

//size of the screen
float2 texelSize;

//textures and sampler for defferred rendering
Texture2D shadowMap;
Texture2D posMap;
Texture2D normalMap;
Texture2D depthMap;
Texture2D occlusionMap;
Texture2D randomTexture;
SamplerState DifferredSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};
SamplerState RandomSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

//Vertex Input
struct VS_INPUT
{
     float4 position : POSITION;
	 float3 normal	: NORMAL;
	 float2 tex0	: TEXCOORD;
	 float4 boneLinks : BONELINKS;
	 float4 boneWeights : BONEWEIGHTS;
};

//blend state to allow transparency
BlendState SrcAlphaBlendingAdd
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};
BlendState DisableBlending
{
	BlendEnable[0] = FALSE;
	RenderTargetWriteMask[0] = 1 | 2 | 4 | 8;
};

//skin a vertex using the matrix palette
float4 skinVert(float4 vert, float4 bones, float4 weights)
{
	float4 p = float4(0.0f, 0.0f, 0.0f, 1.0f);

	int i=0;
	while (bones[i] != -1 && i < 4)
	{
		p += weights[i] * mul(vert,MatrixPalette[bones[i]]);
		i++;
	}
	//vertex skinning
	/*float bone1Weight = fact;
	float bone2Weight = 1.0f - bone1Weight;
	p += bone1Weight * mul(vert,MatrixPalette[bone1]);
	p += bone2Weight * mul(vert,MatrixPalette[bone2]);*/
	p.w = 1.0f;

	return p;
}
//skin a normal using the matrix palette
float3 skinNorm(float3 vert, float4 bones, float4 weights)
{
	float3 norm = float3(0.0f, 0.0f, 0.0f);

	int i=0;
	while (bones[i] != -1 && i < 4)
	{
		norm += weights[i] * mul(vert,MatrixPalette[bones[i]]);
		i++;
	}

	/*float bone1Weight = fact;
	float bone2Weight = 1.0f - bone1Weight;
	norm += bone1Weight * mul(vert,MatrixPalette[bone1]);
	norm += bone2Weight * mul(vert,MatrixPalette[bone2]);*/
	norm = normalize(norm);

	return norm;
}
#endif