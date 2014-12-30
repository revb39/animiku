/************************************************************************
*	AM_Settings.h -- Storage for application settings					*
*					 Copyright (c) 2014 Justin Byers					*
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

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "AMTypes.h"

struct MySettings
{
public:
	/*Number of samples for MSAA*/
	int Samples;
	/*Length (in seconds) of model fades*/
	int FadeLength;
	/*Dimensions (in pixels) of the screen*/
	int Width,Height;
	/*Use alpha fade*/
	bool EcchiFade;
	/*Use MSAA*/
	bool EnableMSAA;
	/*Brightness multiplier for self-lit materials*/
	float ExtraBright;
	/*Rendering FPS*/
	int FPS;
	/*Gravity Multiplier*/
	float Gravity;
	/*Show console*/
	bool DevConsole;
	/*Render shadows*/
	bool Shadows;
	/*Render using SSAO*/
	bool SSAO;
	/*Do a minimal render with no advanced shading*/
	bool Minimal;
	/*Camera default zoom*/
	float Zoom;
	/*Number of samples in SSAO rendering*/
	int SSAOSamples;
	/*SSAO radius*/
	float SSAORad;
	/*SSAO intensity*/
	float SSAOInten;
	/*SSAO scale*/
	float SSAOScl;
	/*SSAO bias*/
	float SSAOBias;
};

class AMSettings {
public:
	AMSettings();

	/*The actual settings structure*/
	MySettings Sets;

	int getNumSamples();
	int getFadeLength();
	int getWidth();
	int getHeight();
	int getFpsCap();
	float getPosStep();
	float getZoomStep();
	float getExtraBright();
	float getGravityMul();
	int getSSAOSamples();
	float getSSAORad();
	float getSSAOInten();
	float getSSAOScale();
	float getSSAOBias();
	bool getEcchi();
	bool getEnabledMSAA();
	bool getAdvShader();
	bool getDevCon();
	bool getShadows();
	bool getSSAO();
	bool getMinimal();
};

#endif