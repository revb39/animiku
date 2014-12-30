/************************************************************************
*	AM_Constraint.h -- Physics engine constraint between rigid bodies	*
*					   Copyright (c) 2014 Justin Byers					*
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

#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include "AMTypes.h"
#include "AM_RigidBody.h"

struct ConstraintInfo
{
	char* Name;
	char* NameEnglish;
	AMRigidBody* BodyA;
	AMRigidBody* BodyB;
	D3DXVECTOR3 InitialPos;
	D3DXVECTOR3 InitialRot;
	D3DXVECTOR3 PosLimitLower;
	D3DXVECTOR3 PosLimitUpper;
	D3DXVECTOR3 RotLimitLower;
	D3DXVECTOR3 RotLimitUpper;
	float Stiffness[6];
};

/*
	Manages the constraints in Bullet Physics between rigid bodies
*/
class AMConstraint {
private:
	/*The constraint info structure*/
	ConstraintInfo _info;
	/*The Bullet constraint object*/
	btGeneric6DofSpringConstraint *_constraint;
	/*Pointer to the Bullet world*/
	btDiscreteDynamicsWorld *_world;
	/*Rigid bodies the constraint acts on*/
	btRigidBody *_bodyA, *_bodyB;

public:
	AMConstraint();
	~AMConstraint();

	/*
		ConstraintInfo* getInfo()

		Get a pointer to the constraint info structure

		Returns:		Pointer to the constraint info structure
	*/
	ConstraintInfo* getInfo();
	/*
		void init()

		Initialize the constraint

		Parameters:		world			Pointer to the Bullet world
	*/
	void init(btDiscreteDynamicsWorld *world);
	/*
		void addToWorld()

		Add this constraint to the Bullet world
	*/
	void addToWorld();
	/*
		void setEnabled()

		Enable/disable this constraint

		Parameters:		e				true/false: constraint is enabled
	*/
	void setEnabled(bool e);
};
#endif