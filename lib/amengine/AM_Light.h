/************************************************************************
*	AM_Light.h -- Rendering matrices for scene lighting					*
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

#ifndef LIGHT_H_
#define LIGHT_H_

#include "AMTypes.h"

/*
	Calculates matrices used in rendering for scene lighting
*/
class AMLight
{
private:
	/*Direction of the light*/
	D3DXVECTOR3 _direction;
	/*Position of the light "fixture"*/
	D3DXVECTOR3 _pos;
	/*Matrices for rendering*/
	D3DXMATRIX _view, _proj;
	/*
		void calcViewMat()

		Calculate view matrix for rendering
	*/
	void calcViewMat();
	/*
		void calcProjMat()

		Calculate projection matrix for rendering
	*/
	void calcProjMat();
public:
	AMLight(D3DXVECTOR3 dir);
	~AMLight();

	/*
		D3DXMATRIX getViewMat()

		Get the view matrix for rendering

		Returns:		The view matrix
	*/
	D3DXMATRIX getViewMat();
	/*
		D3DXMATRIX getProjMat()

		Get the projection matrix for rendering

		Returns:		The projection matrix
	*/
	D3DXMATRIX getProjMat();

	/*
		D3DXVECTOR3 getDirection()

		Get the direction vector of the light

		Returns:		The direction vector of the light
	*/
	D3DXVECTOR3 getDirection();
	/*
		D3DXVECTOR3 getPos()

		Get the world position of the light "fixture"

		Returns:		The world position of the light
	*/
	D3DXVECTOR3 getPos();
};
#endif