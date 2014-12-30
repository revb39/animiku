/************************************************************************
*	AM_BulletPhysics.cpp -- Physics engine								*
*							Copyright (c) 2014 Justin Byers				*
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

#include "AM_BulletPhysics.h"

BulletPhysics::BulletPhysics()
{
	_collisionConfig = NULL;
	_dispatcher = NULL;
	_broadphase = NULL;
	_solver = NULL;
	_world = NULL;
	_fps = 0;
	lastTime = 0;
}
BulletPhysics::~BulletPhysics()
{
	if (_world)
		delete _world;
	if (_solver)
		delete _solver;
	if (_dispatcher)
		delete _dispatcher;
	if (_broadphase)
		delete _broadphase;
	if (_collisionConfig)
		delete _collisionConfig;
}
void BulletPhysics::setup(int simFps, float gMul)
{
	//assign the FPS
	_fps = simFps;

	//set up the collision configuration
	_collisionConfigInfo = btDefaultCollisionConstructionInfo();
	_collisionConfig = new btDefaultCollisionConfiguration(_collisionConfigInfo);
	_collisionConfigInfo.m_useEpaPenetrationAlgorithm = 0;

	//create other things needed for physics
	_dispatcher = new btCollisionDispatcher(_collisionConfig);
	_solver = new btSequentialImpulseConstraintSolver();
	_broadphase = new btDbvtBroadphase();
	
	//create the world & set up gravity
	_world = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfig);
	_world->setGravity(btVector3(0.0f, -9.8f * gMul, 0.0f));

	//set up collision solver
	btContactSolverInfo& solverInfo = _world->getSolverInfo();
	solverInfo.m_solverMode |= SOLVER_ENABLE_FRICTION_DIRECTION_CACHING;
	_world->getSolverInfo().m_numIterations = (int) (10 * 60 / _fps);

	//assign last update time to now
	lastTime = GetTickCount();
}
void BulletPhysics::update()
{
	btScalar sec;

	//get seconds since last update
	sec = btScalar((float)(timeGetTime()-lastTime)*0.001f);
	if (sec == 0)
		return;

	//step the simulation to current time
	_world->stepSimulation(sec, 7);

	//assign last update time to now
	lastTime = timeGetTime();
}
btDiscreteDynamicsWorld* BulletPhysics::getWorld()
{
	return _world;
}


btTransform BulletPhysics::ConvertD3DXMatrix( D3DXMATRIX *d3dMatrix )
{
	btTransform bulletTransformMatrix;
	btVector3 R,U,L,P;

	//do crazy math shit
	R.setX( d3dMatrix->_11 ); R.setY( d3dMatrix->_12 ); R.setZ( d3dMatrix->_13 );
	U.setX( d3dMatrix->_21 ); U.setY( d3dMatrix->_22 ); U.setZ( d3dMatrix->_23 );
	L.setX( d3dMatrix->_31 ); L.setY( d3dMatrix->_32 ); L.setZ( d3dMatrix->_33 );
	P.setX( d3dMatrix->_41 ); P.setY( d3dMatrix->_42 ); P.setZ( d3dMatrix->_43 );

	bulletTransformMatrix.getBasis().setValue( R.x(), U.x(), L.x(), 
											   R.y(), U.y(), L.y(), 
											   R.z(), U.z(), L.z() );
	bulletTransformMatrix.setOrigin( P );
	return bulletTransformMatrix;
}
D3DXMATRIX BulletPhysics::ConvertBTMatrix( btTransform &trn ) 
{
	//do crazy math shit
	btVector3 R = trn.getBasis().getColumn(0);
	btVector3 U = trn.getBasis().getColumn(1);
	btVector3 L = trn.getBasis().getColumn(2);
	btVector3 P = trn.getOrigin();

	D3DXVECTOR3 vR, vU, vL, vP;
	vR.x = R.x();vR.y = R.y();vR.z = R.z();
	vU.x = U.x();vU.y = U.y();vU.z = U.z();
	vL.x = L.x();vL.y = L.y();vL.z = L.z();
	vP.x = P.x();vP.y = P.y();vP.z = P.z();

	D3DXMATRIX matOutput;
	matOutput._11 = vR.x;matOutput._12 = vR.y;matOutput._13 = vR.z;matOutput._14 = 0.f;
	matOutput._21 = vU.x;matOutput._22 = vU.y;matOutput._23 = vU.z;matOutput._24 = 0.f;
	matOutput._31 = vL.x;matOutput._32 = vL.y;matOutput._33 = vL.z;matOutput._34 = 0.f;
	matOutput._41 = vP.x;matOutput._42 = vP.y;matOutput._43 = vP.z;matOutput._44 = 1.f;

	return matOutput;
}





























