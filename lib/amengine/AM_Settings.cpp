/************************************************************************
*	AM_Settings.cpp -- Storate for application settings					*
*					   Copyright (c) 2014 Justin Byers					*
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

#include "AM_Settings.h"

AMSettings::AMSettings()
{
	Sets.Samples = 1;
	Sets.FadeLength = 1;
	Sets.Width = 1024;
	Sets.Height = 768;
	Sets.ExtraBright = 1.0f;
	Sets.FPS = 60;
	Sets.EcchiFade = false;
	Sets.EnableMSAA = false;
	Sets.DevConsole = false;
	Sets.Shadows = false;
	Sets.SSAO = false;
	Sets.Minimal = false;

	Sets.SSAOSamples = 16;
	Sets.SSAORad = 2;
	Sets.SSAOInten = 1;
	Sets.SSAOScl = 0.8f;
	Sets.SSAOBias = 0.01f;
}

bool AMSettings::getEcchi()
{
	return Sets.EcchiFade;
}
int AMSettings::getFadeLength()
{
	return Sets.FadeLength;
}
int AMSettings::getNumSamples()
{
	return Sets.Samples;
}
int AMSettings::getWidth()
{
	return Sets.Width;
}
int AMSettings::getHeight()
{
	return Sets.Height;
}
int AMSettings::getFpsCap()
{
	return Sets.FPS;
}
float AMSettings::getExtraBright()
{
	return Sets.ExtraBright;
}
float AMSettings::getGravityMul()
{
	return Sets.Gravity;
}
int AMSettings::getSSAOSamples()
{
	return Sets.SSAOSamples;
}
float AMSettings::getSSAORad()
{
	return Sets.SSAORad;
}
float AMSettings::getSSAOInten()
{
	return Sets.SSAOInten;
}
float AMSettings::getSSAOScale()
{
	return Sets.SSAOScl;
}
float AMSettings::getSSAOBias()
{
	return Sets.SSAOBias;
}
bool AMSettings::getEnabledMSAA()
{
	return Sets.EnableMSAA;
}
bool AMSettings::getDevCon()
{
	return Sets.DevConsole;
}
bool AMSettings::getShadows()
{
	return Sets.Shadows;
}
bool AMSettings::getSSAO()
{
	return Sets.SSAO;
}
bool AMSettings::getMinimal()
{
	return Sets.Minimal;
}