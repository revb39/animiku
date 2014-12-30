/************************************************************************
*	AM_Light.cpp -- Rendering matrices for scene lighting				*
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

#include "AM_Light.h"

AMLight::AMLight(D3DXVECTOR3 dir)
{
	_direction = dir;
	//put fixture in position to mimick overhead stage lighting
	//and make shadows more visible
	_pos = D3DXVECTOR3(0, 30, 50);
	calcViewMat();
	calcProjMat();
}
AMLight::~AMLight()
{

}

void AMLight::calcViewMat()
{
	//calculate view matrix
	D3DXMatrixLookAtLH(&_view, &_pos, &D3DXVECTOR3(0,10,0), &D3DXVECTOR3(0,1,0));
}
void AMLight::calcProjMat()
{
	//calculate projection matrix
	D3DXMatrixOrthoLH(&_proj, 100, 62.5, 1.0f, 400.0f);
}

D3DXMATRIX AMLight::getViewMat()
{
	return _view;
}
D3DXMATRIX AMLight::getProjMat()
{
	return _proj;
}
D3DXVECTOR3 AMLight::getDirection()
{
	return _direction;
}
D3DXVECTOR3 AMLight::getPos()
{
	return _pos;
}