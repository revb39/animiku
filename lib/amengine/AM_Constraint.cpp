/************************************************************************
*	AM_Constraint.cpp -- Physics engine constraint between rigid bodies *
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

#include "AM_Constraint.h"

AMConstraint::AMConstraint()
{
	ZeroMemory(&_info, sizeof(ConstraintInfo));
	_constraint = NULL;
	_world = NULL;
	_bodyA = NULL;
	_bodyB = NULL;
}
AMConstraint::~AMConstraint()
{
	if (_constraint)
	{
		_world->removeConstraint(_constraint);
		delete _constraint;
	}

	if (_info.Name)
		delete[] _info.Name;
	if (_info.NameEnglish)
		delete[] _info.NameEnglish;
}

ConstraintInfo* AMConstraint::getInfo()
{
	return &_info;
}
void AMConstraint::init(btDiscreteDynamicsWorld *world)
{
	btTransform tr;
	btQuaternion rot;

	//don't do anything if either body wasn't provided
	if (_info.BodyA == NULL || _info.BodyB == NULL)
		return;
	
	//assign the world & rigid bodies
	_world = world;
	_bodyA = _info.BodyA->getBody();
	_bodyB = _info.BodyB->getBody();

	tr.setIdentity();
	//set up transform with initial rotation and position
	rot.setEuler(_info.InitialRot.y, _info.InitialRot.x, _info.InitialRot.z);
	tr.setOrigin(btVector3(_info.InitialPos.x, _info.InitialPos.y, _info.InitialPos.z));
	tr.setRotation(rot);

	//get the bodies' transforms
	btTransform trA = _bodyA->getWorldTransform().inverse() * tr;
	btTransform trB = _bodyB->getWorldTransform().inverse() * tr;
	
	//build the constraint
	_constraint = new btGeneric6DofSpringConstraint(*_bodyA, *_bodyB, trA, trB, true);

	//set the linear limits
	_constraint->setLinearLowerLimit(btVector3(_info.PosLimitLower.x, _info.PosLimitLower.y, _info.PosLimitLower.z));
	_constraint->setLinearUpperLimit(btVector3(_info.PosLimitUpper.x, _info.PosLimitUpper.y, _info.PosLimitUpper.z));

	//set the angular limits
	_constraint->setAngularLowerLimit(btVector3(_info.RotLimitLower.x, _info.RotLimitLower.y, _info.RotLimitLower.z));
	_constraint->setAngularUpperLimit(btVector3(_info.RotLimitUpper.x, _info.RotLimitUpper.y, _info.RotLimitUpper.z));

	//enable all springs and set the stiffness
	for (int i=0;i<6;i++)
	{
		if (i >= 3 || _info.Stiffness[i] != 0.0f)
		{
			_constraint->enableSpring(i, true);
			_constraint->setStiffness(i, _info.Stiffness[i]);
		}
	}
}

void AMConstraint::addToWorld()
{
	if (!_constraint)
		return;

	_world->addConstraint(_constraint);
}
void AMConstraint::setEnabled(bool e)
{
	//"enabled" defined as the constraint existing in the world
	if (e)
		_world->addConstraint(_constraint);
	else
		_world->removeConstraint(_constraint);
}