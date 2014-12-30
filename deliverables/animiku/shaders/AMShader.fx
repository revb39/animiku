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

#include "MapGen.fx"
#include "MainRender.fx"

technique10 Skinning
{
	pass P0
	{
		SetBlendState(SrcAlphaBlendingAdd, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );
		SetGeometryShader( NULL );
	}
}

technique10 ShadowMapT
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, vs_Shadow() ) );
        SetPixelShader( CompileShader( ps_4_0, ps_Shadow() ) );
		SetGeometryShader( NULL );
	}
}

technique10 SSAO_T
{
	pass P0
	{
		SetBlendState(DisableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader( CompileShader( vs_4_0, VS_Dif() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_Dif() ) );
		SetGeometryShader( NULL );
	}
}

technique10 SSAO_OCC_T
{
	pass P0
	{
		SetBlendState(DisableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader( CompileShader( vs_4_0, VS_Dif() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_DifOcc() ) );
		SetGeometryShader( NULL );
	}
}

technique10 BLUR1_T
{
	pass P0
	{
		SetBlendState(DisableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader( CompileShader( vs_4_0, VS_Dif() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_DifBlur() ) );
		SetGeometryShader( NULL );
	}
}