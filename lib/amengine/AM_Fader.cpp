/************************************************************************
*	AM_Fader.cpp -- Time fade to fade models in and out					*
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

#include "AM_Fader.h"

AMFader::AMFader()
{
	_length = 0;
	_isFadingOut = false;
	_startTime = 0;
}

void AMFader::BeginFade(int length, bool isFadingOut)
{
	_length = (length*1000);
	_isFadingOut = isFadingOut;
	_startTime = timeGetTime();
}
float AMFader::GetCurAlphaScalar()
{
	//get the time delta since the lsat update
	DWORD deltaTime = timeGetTime()-_startTime;

	//use basic linear interpolation to calculate the scalar
	if (_isFadingOut)
	{
		//check limit
		if (deltaTime >= _length)
			return 0;
		DWORD timeLeft = _length-deltaTime;
		return ((float)timeLeft/(float)_length);
	}	
	else
	{
		//check limit
		if (deltaTime >= _length)
			return 1;
		return ((float)deltaTime/(float)_length);
	}
}
bool AMFader::GetFadingDone()
{
	//fading is complete if time delta >= fade length
	DWORD deltaTime = timeGetTime()-_startTime;
	return (deltaTime >= _length);
}