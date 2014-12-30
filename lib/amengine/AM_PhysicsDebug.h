/************************************************************************
*	AM_PhysicsDebug.h -- Bullet Physics debug rendering					*
*						 Copyright (c) 2014 Justin Byers				*
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

#ifndef PHYSDEBUG_H_
#define PHYSDEBUG_H_

#include "AMTypes.h"
#include "btBulletDynamicsCommon.h"

/*
	btIDebugDraw implementation to allow Bullet to render rigid bodies for debugging
	using AMENGINE
*/
class AMPhysicsDebug : public btIDebugDraw
{
private:
	/*Pointer to the D3D device*/
	ID3D10Device *_dev;
	/*Pointer to the rendering technique*/
	ID3D10EffectTechnique *_tech;
	/*The vertex buffer that will be used for rendering*/
	ID3D10Buffer *_vBuf;
	/*The Bullet debug drawing mode*/
	DebugDrawModes _dbgMode;
	/*The vertices used in rendering the bodies*/
	MMDVERTEX *_v;

	/*
		void bufInit()

		Initialize the vertex buffer
	*/
	void bufInit();
public:
	AMPhysicsDebug(ID3D10Device *dev, ID3D10EffectTechnique *tech);
	~AMPhysicsDebug();

	/*All below functions are defined by the Bullet Physics btIDebugDraw interface*/
	virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
	virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
	virtual void reportErrorWarning (const char *warningString);
	virtual void draw3dText (const btVector3 &location, const char *textString);
	virtual void setDebugMode (int debugMode);
    virtual int  getDebugMode () const;
};

#endif