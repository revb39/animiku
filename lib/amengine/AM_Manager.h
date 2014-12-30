/************************************************************************
*	AM_Manager.h -- Interface for UI elements and engine events			*
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

#ifndef _AM_MANAGER_H_
#define _AM_MANAGER_H_

#include "AMTypes.h"

/*
	An interface meant to be implemented by a UI or management engine.
	Rendering engine will call these events.
*/
class AM_Manager
{
public:
	/*
		virtual void songEnd()

		Called by AMENGINE when a song ends
	*/
	virtual void songEnd() = 0;
	/*
		virtual void fadedOut()

		Called by AMENGINE when a model is faded out
	*/
	virtual void fadedOut() = 0;
};

#endif