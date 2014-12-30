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

#ifndef _BLUR_FX_
#define _BLUR_FX_

#include "Common.fx"

float4 weight = {
    0.8,
    0.65,
    0.4,
    0.25
};
     
float3 blurHor(Texture2D source, float2 Tex, float2 rcpres)
{ // a simple 7-sample horizontal blur pass  
        float4 Color = source.Sample(DifferredSampler, Tex);
     
        Color += source.Sample(DifferredSampler, float2(Tex.x-(rcpres.x*1), Tex.y)) * weight[0];
        Color += source.Sample(DifferredSampler, float2(Tex.x+(rcpres.x*1), Tex.y)) * weight[0];
     
        Color += source.Sample(DifferredSampler, float2(Tex.x-(rcpres.x*2), Tex.y)) * weight[1];
        Color += source.Sample(DifferredSampler, float2(Tex.x+(rcpres.x*2), Tex.y)) * weight[1];
     
        Color += source.Sample(DifferredSampler, float2(Tex.x-(rcpres.x*3), Tex.y)) * weight[2];
        Color += source.Sample(DifferredSampler, float2(Tex.x+(rcpres.x*3), Tex.y)) * weight[2];
     
        Color += source.Sample(DifferredSampler, float2(Tex.x-(rcpres.x*4), Tex.y)) * weight[3];
        Color += source.Sample(DifferredSampler, float2(Tex.x+(rcpres.x*4), Tex.y)) * weight[3];
     
        Color /= 5.0f;
     
        return Color.xyz;
}
     
float3 blurVert(Texture2D source, float2 Tex, float2 rcpres)
{ // a simple 7-sample vertical blur pass
        float4 Color = source.Sample(DifferredSampler, Tex);
     
        Color += source.Sample(DifferredSampler, float2(Tex.x, Tex.y-(rcpres.y*1))) * weight[0];
        Color += source.Sample(DifferredSampler, float2(Tex.x, Tex.y+(rcpres.y*1))) * weight[0];
     
        Color += source.Sample(DifferredSampler, float2(Tex.x, Tex.y-(rcpres.y*2))) * weight[1];
        Color += source.Sample(DifferredSampler, float2(Tex.x, Tex.y+(rcpres.y*2))) * weight[1];
     
        Color += source.Sample(DifferredSampler, float2(Tex.x, Tex.y-(rcpres.y*3))) * weight[2];
        Color += source.Sample(DifferredSampler, float2(Tex.x, Tex.y+(rcpres.y*3))) * weight[2];
     
        Color += source.Sample(DifferredSampler, float2(Tex.x, Tex.y-(rcpres.y*4))) * weight[3];
        Color += source.Sample(DifferredSampler, float2(Tex.x, Tex.y+(rcpres.y*4))) * weight[3];
     
        Color /= 5.0f;
     
        return Color.xyz;
}
#endif