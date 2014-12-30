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
#include "ShadowDepthGen.fx"
#include "Blur.fx"

matrix lightViewMatrix;

//constants
float ambientBright = 1;
float diffuseBright = 0.5;
float specularBright = 0.05;

//vectors
float3 lightDir = (0.0, 0.0, -1.0);
float3 lightPos;
float3 cameraPos;

//state variables
bool useTexture;
bool useSphere;
int sphereType;
bool shadows;
bool SSAO;
bool renderingSkel;
bool renderingRBody;

bool minimal;

//material color info
float4 materialAmbient;
float4 materialDiffuse;
float4 materialSpecular;
float shine;
float alpha;
float extraBright = 1.0;
float fadeCoef;

//Textures
Texture2D texDiffuse;
Texture2D sphTexture;

//samplers
SamplerState DiffuseSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


//Vertex Output / Pixel Shader Input
struct VS_OUTPUT
{
     float4 position : SV_POSITION;
	 float3 normal	: NORMAL;
	 float3 eye	: TEXCOORD0;
	 float2 tex0	:TEXCOORD2;
	 float2 spTex	: TEXCOORD3;
	 float4 lpos : TEXCOORD4;
};


VS_OUTPUT VS_Main(VS_INPUT i)
{
	VS_OUTPUT o;

	float4 p;
	float3 n;
	if (renderingRBody)
	{
		p = i.position;
		n = i.normal;
		o.lpos.xyz = i.normal;
	}
	else
	{
		p = skinVert(i.position, i.boneLinks, i.boneWeights);
		n = skinNorm(i.normal, i.boneLinks, i.boneWeights);
		o.lpos = mul(p, mul(worldMatrix, lightViewProjMatrix));
	}

	if (renderingSkel)
		p = i.position;
	

	float4 worldPos;
	worldPos = mul(p, worldMatrix);

	o.position = mul(worldPos, viewMatrix);
	o.position = mul(o.position, projMatrix);

	o.tex0 = i.tex0;

	o.normal = mul(n, worldMatrix);
	o.normal = normalize(mul(o.normal, lightViewMatrix));
	
	o.eye = lightPos-worldPos;

	if (useSphere)
	{
		float2 normalWV = mul(n, (float3x3)viewMatrix);
		o.spTex.x = normalWV.x * 0.5f + 0.5f;
		o.spTex.y = normalWV.y * -0.5f + 0.5f;
	}

	return o;
}

float4 PS_Main(VS_OUTPUT i) : SV_Target
{
	if (renderingRBody)
	{
		//for rigid bodies, we put the color in the lpos variable
		return float4(i.lpos.xyz,1);
	}
	else if (renderingSkel)
	{
		//for skeletal rendering, just use white
		return float4(1,1,1,1);
	}
	else if (!minimal)
	{
		//return float4((i.normal+1)/2.0f, 1);
		//calculate screen space UV coordinates
		float2 uv = (i.position.xy) / texelSize;

		//return getOcclusion(uv);

		float4 aof = float4(1,1,1,1);
		if (SSAO)
		{
			//aof = float4(occlusionMap.Sample(DifferredSampler, i.position.xy/texelSize).xyz,1);
			aof.xyz = blurVert(occlusionMap, i.position.xy/texelSize, 1/texelSize);
		}

		float4 ambient = ambientBright*materialAmbient;
		if (SSAO)
			ambient *= aof;
		ambient.a = 1;

		float3 V = normalize(i.eye - i.position);
		float3 R = reflect(lightDir, i.normal);

		float4 diffuse = diffuseBright*materialDiffuse*saturate(dot(lightDir,i.normal));
		if (shadows)
			diffuse *= getShadowFactor(i.lpos);
		float4 specular = specularBright*pow(saturate(dot(R,V)),normalize(shine));

		float4 finalColor = ambient+diffuse;

		if (useTexture)
			finalColor *= texDiffuse.Sample(DiffuseSampler, i.tex0);

		switch (sphereType)
		{
		case 0:
			//no sphere
			break;
		case 1:
			//multiply	
			finalColor *= sphTexture.Sample(DiffuseSampler, i.spTex);
			break;
		case 2:
			//add
			finalColor += sphTexture.Sample(DiffuseSampler, i.spTex);
			break;
		case 3:
			//subtract
			finalColor -= sphTexture.Sample(DiffuseSampler, i.spTex);
			break;
		}
		

		if (shine > 0)
			finalColor = saturate(finalColor+specular);

		finalColor *= extraBright*fadeCoef;

		if (useTexture)
			finalColor.a = texDiffuse.Sample(DiffuseSampler, i.tex0).a * materialDiffuse.a;
		else
			finalColor.a = materialDiffuse.a;
	
		return finalColor;
	}
	else
	{
		if (minimal)
		{
			if (useTexture)
				return float4(texDiffuse.Sample(DiffuseSampler, i.tex0).rgb*fadeCoef, texDiffuse.Sample(DiffuseSampler, i.tex0).a);
			else
				return materialDiffuse;
		}
	}
}