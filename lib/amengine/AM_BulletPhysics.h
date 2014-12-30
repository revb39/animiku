/************************************************************************
*	AM_BulletPhysics.h -- Physics engine								*
*						  Copyright (c) 2014 Justin Byers				*
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

#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include "AMTypes.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision\CollisionShapes\btShapeHull.h"
#include "BulletCollision\CollisionShapes\btConvexShape.h"

/*
	Physics engine internal to AniMiku uses the Bullet Physics engine
	to perform all calculations
*/
class BulletPhysics {
private:
	/*All objects needed to use Bullet*/
	btDefaultCollisionConstructionInfo _collisionConfigInfo;
	btDefaultCollisionConfiguration *_collisionConfig;
	btCollisionDispatcher *_dispatcher;
	btBroadphaseInterface *_broadphase;
	btConstraintSolver *_solver;
	btDiscreteDynamicsWorld *_world;

	/*FPS of the physics simulation*/
	int _fps;
	/*Time of last engine update*/
	DWORD lastTime;
	
public:
	BulletPhysics();
	~BulletPhysics();

	/*
		void setup()

		Set up and initialize the physics engine

		Parameters:		simFps			FPS of the physics simulation
						gMul			Gravity multiplier (10 matches MMD)
	*/
	void setup(int simFps, float gMul);

	/*
		void update()

		Update the physics engine to match the current time delta
	*/
	void update();

	/*
		btDiscreteDynamicsWorld* getWorld()

		Get pointer to the physics world

		Returns:		pointer to the physics world
	*/
	btDiscreteDynamicsWorld *getWorld();

	/*
		btTransform ConvertD3DXMatrix()

		Convert a D3DXMATRIX to btTransform

		Parameters:		d3dMatrix		The matrix to convert
		Returns:		btTransform		The converted matrix
	*/
	static btTransform ConvertD3DXMatrix( D3DXMATRIX *d3dMatrix );
	/*
		D3DXMATRIX ConvertBTMatrix()

		Convert a btTransform to D3DXMATRIX

		Parameters:		trn				The matrix to convert
		Returns:		D3DXMATRIX		The converted matrix
	*/
	static D3DXMATRIX ConvertBTMatrix( btTransform &trn ) ;
};
#endif