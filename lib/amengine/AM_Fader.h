/************************************************************************
*	AM_Fader.h -- Time fade to fade models in and out					*
*				  Copyright (c) 2014 Justin Byers						*
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

#ifndef FADER_H_
#define FADER_H_

#include "AMTypes.h"

/*
	Interpolate scalars used to fade textures and materials
*/
class AMFader
{
private:
	/*Time of fade start*/
	DWORD _startTime;
	/*Length (in seconds) of the fade*/
	DWORD _length;
	/*Are we fading out?*/
	bool _isFadingOut;
public:
	AMFader();

	/*
		void BeginFade()

		Start fading

		Parameters:			length			Length (in seconds) of the fade
							isFadingOut		Is this fading out?
	*/
	void BeginFade(int length, bool isFadingOut);
	/*
		float GetCurAlphaScalar()

		Get the current alpha scalar from the fader (0.0-1.0)

		Returns:			float			Current alpha scalar
	*/
	float GetCurAlphaScalar();

	/*
		bool GetFadingDone()

		Get if the fade is complete

		Returns:			bool			The fade is complete
	*/
	bool GetFadingDone();
};
#endif