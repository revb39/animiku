/************************************************************************
*	AM_RigidBody.cpp -- Bullet physics rigid body management			*
*						Copyright (c) 2014 Justin Byers					*
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

#include "AM_RigidBody.h"

AMRigidBody::AMRigidBody()
{
	ZeroMemory(&_info, sizeof(RigidBodyInfo));
	_shape = NULL;
	_body = NULL;
	_motionState = NULL;
	_kinematicMotionState = NULL;
	_world = NULL;
}
AMRigidBody::~AMRigidBody()
{
	if (_body)
	{
		_world->removeCollisionObject(_body);
		delete _body;
	}
	if (_shape)
		delete _shape;

	if (_motionState)
		delete _motionState;
	if (_kinematicMotionState && _motionState != _kinematicMotionState)
		delete _kinematicMotionState;

	if (_info.Name)
		delete[] _info.Name;
	if (_info.NameEnglish)
		delete[] _info.NameEnglish;
}

RigidBodyInfo* AMRigidBody::getInfo()
{
	return &_info;
}

bool AMRigidBody::init(btDiscreteDynamicsWorld* world)
{
	_world = world;

	//create the shape that will be used for physics calulations
	switch (_info.Shape)
	{
	case RigidBodyDefs::Sphere:
		_shape = new btSphereShape(_info.BodySize.x);
		break;
	case RigidBodyDefs::Box:
		_shape = new btBoxShape(btVector3(_info.BodySize.x, _info.BodySize.y, _info.BodySize.z));
		break;
	case RigidBodyDefs::Capsule:
		_shape = new btCapsuleShape(_info.BodySize.x, _info.BodySize.y);
		break;
	default:
		return false;
	}

	//no mass for kinematic objects
	if (_info.Type == RigidBodyDefs::Kinematics)
		_info.PhysConstants.Mass = 0.0f;

	btVector3 localInertia = btVector3(0.0,0.0,0.0);
	//only do this calculation if the rigid body is simulated
	if (_info.PhysConstants.Mass != 0.0f)
		_shape->calculateLocalInertia(btScalar(_info.PhysConstants.Mass), localInertia);

	//initialize the transform with the initial pos and rot
	_trans.setIdentity();
	_localRot.setEuler(_info.RelativeRot.y, _info.RelativeRot.x, _info.RelativeRot.z); //create the rotation quaternion
	_trans.setOrigin(btVector3(_info.RelativePos.x, _info.RelativePos.y, _info.RelativePos.z)); //move the transform to the local postion
	_trans.setRotation(_localRot); //rotate the transform by the local rotation
	//get the inverse transform of the body
	_transInv = _trans.inverse();

	btTransform startTrans = btTransform::getIdentity();
	startTrans.setOrigin(btVector3(_info.RelatedBone->getCombinedTrans()(3,0), _info.RelatedBone->getCombinedTrans()(3,1), _info.RelatedBone->getCombinedTrans()(3,2)));
	startTrans *= _trans; //calculate the rigid body's world transform by combining it's local transform with the bone's combined transform

	//create the motion state depending on what type of body this is
	_kinematicMotionState = new KinematicMotionState(startTrans, _trans, _info.RelatedBone);

	switch (_info.Type)
	{
	case RigidBodyDefs::Kinematics:
		_motionState = _kinematicMotionState;
		break;
	case RigidBodyDefs::Simulated:
		_motionState = new btDefaultMotionState(startTrans);
		break;
	case RigidBodyDefs::SimulatedAligned:
		_motionState = new AlignedMotionState(startTrans, _trans, _info.RelatedBone);
		break;
	default:
		return false;
	}

	//create the rigid body
	btRigidBody::btRigidBodyConstructionInfo rbInfo(_info.PhysConstants.Mass, _motionState, _shape, localInertia);
	rbInfo.m_linearDamping = _info.PhysConstants.LinearDamping;
	rbInfo.m_angularDamping = _info.PhysConstants.AngularDamping;
	rbInfo.m_restitution = _info.PhysConstants.Restitution;
	rbInfo.m_friction = _info.PhysConstants.Friction;
	rbInfo.m_additionalDamping = true;

	_body = new btRigidBody(rbInfo);
	_body->setMotionState(_motionState);
	
	//mark the body kinematic (move with bone) if that is the type
	if (_info.Type == RigidBodyDefs::Kinematics)
		_body->setCollisionFlags(_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

	_body->setActivationState(DISABLE_DEACTIVATION);

	_info.Group = 1 << _info.Group; //ready the group identifier for use with bulletphysics

	return true;
}

void AMRigidBody::setEnabled(bool e)
{
	if (e)
	{
		_body->setCollisionFlags(_body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
		if (!_body->isInWorld())
			_world->addRigidBody(_body, _info.Group, _info.NonCollisionMask);
	}
	else
	{
		_body->setCollisionFlags(_body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		_world->removeRigidBody(_body);
	}
}

void AMRigidBody::addToWorld()
{
	if (!_body)
		return;

	_world->addRigidBody(_body, _info.Group, _info.NonCollisionMask);
}

void AMRigidBody::applyTransformToBone()
{
	btTransform tr;
	btVector3 newPos;
	btQuaternion newRot;
	D3DXQUATERNION rQ;
	D3DXMATRIX r, p, f;
	float pos[3], rot[4];
	if (_info.Type == RigidBodyDefs::Kinematics || _info.RelatedBone == NULL)
		return;

	tr = _body->getCenterOfMassTransform(); //get the current position of the body
	tr *= _transInv; //move the transform back to bone space
	
	//get the postion and rotation into something AMBone can use
	newPos = tr.getOrigin();
	newRot = tr.getRotation();

	pos[0] = newPos.x();
	pos[1] = newPos.y();
	pos[2] = newPos.z();

	rot[0] = newRot.x();
	rot[1] = newRot.y();
	rot[2] = newRot.z();
	rot[3] = newRot.w();
	rQ = rot;

	//build the final transformation matrix
	D3DXMatrixRotationQuaternion(&r, &rQ);
	D3DXMatrixTranslation(&p, pos[0], pos[1], pos[2]);
	D3DXMatrixMultiply(&f, &r, &p);

	//update the bone rotation and combined transform
	_info.RelatedBone->setRot(rQ);
	_info.RelatedBone->setCombTrans(f);
}
void AMRigidBody::updatePosition()
{
	_world->removeRigidBody(_body); //remove the body from the world to avoid unnecessary issues

	//get the bone's world transform and move it into body space by multiplying by the local transform of the body
	btTransform tr = BulletPhysics::ConvertD3DXMatrix(&(_info.RelatedBone->getCombinedTrans()));
	tr *= _trans;

	_body->setCenterOfMassTransform(tr); //change the body's transform
	_world->addRigidBody(_body, _info.Group, _info.NonCollisionMask); //re-add it to the world with the new transform

	//clear all physics constants so it doesn't bounce around or do something weird
	_body->setLinearVelocity( btVector3( 0.0f, 0.0f, 0.0f ) );
	_body->setAngularVelocity( btVector3( 0.0f, 0.0f, 0.0f ) );
	_body->setInterpolationLinearVelocity( btVector3( 0.0f, 0.0f, 0.0f ) );
	_body->setInterpolationAngularVelocity( btVector3( 0.0f, 0.0f, 0.0f ) );
	_body->setInterpolationWorldTransform( tr );
	_body->clearForces();
	_body->updateInertiaTensor();
}
btRigidBody* AMRigidBody::getBody()
{
	return _body;
}

//
// AlignedMotionState
//
AlignedMotionState::AlignedMotionState(const btTransform &startTrans, const btTransform &boneTrans, AMBone* bone)
{
	_bone = bone;
	_boneTrans = boneTrans;
	_boneTransInv = boneTrans.inverse();
	_graphicsWorldTrans = startTrans;
}
AlignedMotionState::~AlignedMotionState()
{
}
void AlignedMotionState::getWorldTransform(btTransform &worldTrans) const
{
   worldTrans = _graphicsWorldTrans;
}
void AlignedMotionState::setWorldTransform(const btTransform &worldTrans)
{
   btMatrix3x3 bm;

   _graphicsWorldTrans = worldTrans;
   bm = _graphicsWorldTrans.getBasis();
   _graphicsWorldTrans.setOrigin(btVector3(0.0f, 0.0f, 0.0f));
   _graphicsWorldTrans = _boneTrans * _graphicsWorldTrans;
   _graphicsWorldTrans.setOrigin(_graphicsWorldTrans.getOrigin() + btVector3(_bone->getCombinedTrans()(3,0),_bone->getCombinedTrans()(3,1),_bone->getCombinedTrans()(3,2)));
   _graphicsWorldTrans.setBasis(bm);
}
void AlignedMotionState::setWorldTransformDirectly(const btTransform &worldTrans)
{
   _graphicsWorldTrans = worldTrans;
}

//
// KinematicMotionState
//
KinematicMotionState::KinematicMotionState(const btTransform &startTrans, const btTransform &boneTrans, AMBone *bone)
{
   _bone = bone;
   _boneTrans = boneTrans;
}
/* KinematicMotionState::~KinematicMotionState: destructor */
KinematicMotionState::~KinematicMotionState()
{
}
/* KinematicMotionState::getWorldTransform: get world transform */
void KinematicMotionState::getWorldTransform(btTransform &worldTrans) const
{
	worldTrans = BulletPhysics::ConvertD3DXMatrix(&(_bone->getCombinedTrans())) * _boneTrans;
}
/* KinematicMotionState::setWorldTransform: set world transform */
void KinematicMotionState::setWorldTransform(const btTransform &worldTrans)
{
   /* kinematic objects will not be controlled by physics simulation, just ignore this */
}





