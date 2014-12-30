/************************************************************************
*	AM_Camera.cpp -- Virtual camera for rendering						*
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

#include "AM_Camera.h"

Camera::Camera(int screenWidth, int screenHeight)
{
	D3DXMATRIX matTrans, matRotX, matRotY, matRotZ;

	//initialize stuff
	_pos = D3DXVECTOR3(0, 10, 300);
	_rot = D3DXVECTOR3(0,0,0);
	_target = D3DXVECTOR3(0,0,0);
	z = 0.93f;
	//calculate aspect ratio
	aspect = (float)screenWidth/(float)screenHeight;
	//we don't want to render in perspective
	perspective = false;
	//calculate initial view matrix
	D3DXMatrixTranslation(&matTrans, _pos.x, _pos.y, _pos.z);
	D3DXMatrixRotationX(&matRotX, _rot.x);
	D3DXMatrixRotationY(&matRotY, _rot.y);
	D3DXMatrixRotationZ(&matRotZ, _rot.z);
	matView = (matRotX * matRotY * matRotZ * matTrans);
	//calculate the initial projection matrix
	updateProj();
}
Camera::~Camera()
{
	
}
void Camera::updateProj()
{
	//how the projection matrix is calculated depends of perspective or othographic rendering
	if (perspective)
		D3DXMatrixPerspectiveFovLH(&matProjection, 45*z, aspect, 1.0, 200.0);
	else
		D3DXMatrixOrthoLH(&matProjection, 30/z, (30/z)/aspect, 1.0, 400.0);
}

void Camera::lookAt(float x, float y, float z)
{
	_target = D3DXVECTOR3(x, y, z);
	D3DXMatrixLookAtLH(&matView,
			&_pos,
			&_target,
			&D3DXVECTOR3 (0.0f, 1.0f, 0.0f));
}
void Camera::lookAtFromPos(float x, float y, float z, float cx, float cy, float cz)
{
	_pos = D3DXVECTOR3(cx, cy, cz);
	_target = D3DXVECTOR3(x, y, z);

	D3DXMatrixLookAtLH(&matView,
			&_pos,
			&_target,
			&D3DXVECTOR3 (0.0f, 1.0f, 0.0f));
}
void Camera::zoom(float percent, bool in)
{
	if (!in)
		z -= (z * percent);
	else
		z += (z * percent);

	updateProj();
}

void Camera::setZoom(float zoom)
{
	z = zoom;
	updateProj();
}
float Camera::getZoom()
{
	return z;
}

D3DXVECTOR3 Camera::getPos()
{
	return _pos;
}
D3DXVECTOR3 Camera::getTarget()
{
	return _target;
}