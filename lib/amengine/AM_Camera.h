/************************************************************************
*	AM_Camera.h -- Virtual camera for rendering							*
*				   Copyright (c) 2014 Justin Byers						*
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

#ifndef CAMERA_H_
#define CAMERA_H_
#include "AMTypes.h"

/*
	Manages view and projection matrices for rendering
*/
class Camera {
private:
	/*Will the camers render in perspective?*/
	bool perspective;
	/*Position and rotation of the camera*/
	D3DXVECTOR3 _pos, _rot;
	/*Target point that the camera is looking at*/
	D3DXVECTOR3 _target;
	/*Camera zoom*/
	float z;
	/*Camera aspect ratio*/
	float aspect;

	/*
		void updateProj()

		Update the projection matrix for rendering
	*/
	void updateProj();

public:
	Camera(int screenWidth, int screenHeight);
	~Camera();

	/*Rendering view matrix*/
	D3DXMATRIX matView;
	/*Rendering projection matrix*/
	D3DXMATRIX matProjection;

	/*
		D3DXVECTOR3 getPos()

		Get the world position of the camera

		Returns:		World position of the camera
	*/
	D3DXVECTOR3 getPos();
	/*
		D3DXVECTOR3 getTarget()

		Get the world position the camera is looking at

		Returns:		World position that camera is looking at
	*/
	D3DXVECTOR3 getTarget();
	/*
		float getZoom()

		Get the zoom of the camera

		Returns:		Zoom of the camera
	*/
	float getZoom();

	/*
		void zoom()

		Zoom the camera in or out. Calculated using this formula: zoom = zoom +- (zoom * percent)

		Parameters:		percent				Percent (0.0-1.0) to zoom
						in					true: zoom in, false: zoom out
	*/
	void zoom(float percent, bool in);
	/*
		void setZoom()

		Set the zoom directly

		Parameters:		zoom				Zoom to set (0.0-1.0)
	*/
	void setZoom(float zoom);

	/*
		void lookAt()

		Look at a target point from current position

		Parameters:		x,y,z				World coordinates of target point
	*/
	void lookAt(float x, float y, float z);
	/*
		void lookAtFromPos()

		Look at a target point from a given position

		Parameters:		x,y,z				World coordinates of target point
						cx,cy,cz			World coordinates of camera position
	*/
	void lookAtFromPos(float x, float y, float z, float cx, float cy, float cz);
};
#endif