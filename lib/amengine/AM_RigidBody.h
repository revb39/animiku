/************************************************************************
*	AM_RigidBody.h -- Bullet Physics rigid body management				*
*					  Copyright (c) 2014 Justin Byers					*
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

#ifndef RBODY_H_
#define RBODY_H_

#include "AMTypes.h"
#include "AM_IK.h"
#include "AM_BulletPhysics.h"

/*Type definitions used by this class*/
struct RigidBodyDefs
{
	typedef enum RigidBodyType {Kinematics, Simulated, SimulatedAligned};
	typedef enum RigidBodyShape {Sphere=0,Box,Capsule};
};
/*Constants used in body calculations*/
struct PhysicsConstants
{
	float Mass;
	float LinearDamping;
	float AngularDamping;
	float Restitution;
	float Friction;
};
/*Rigid body info structure*/
struct RigidBodyInfo
{
	char* Name;
	char* NameEnglish;
	RigidBodyDefs::RigidBodyType Type;
	RigidBodyDefs::RigidBodyShape Shape;

	D3DXVECTOR3 BodySize;

	PhysicsConstants PhysConstants;

	D3DXVECTOR3 RelativePos;
	D3DXVECTOR3 RelativeRot;

	AMBone* RelatedBone;

	unsigned char Group;
	unsigned short NonCollisionMask;
};

/*
	Manages all calculations used for physics rigid bodies
*/
class AMRigidBody {
private:
	/*The rigid body info structure*/
	RigidBodyInfo _info;

	/*Shape of the body*/
	btCollisionShape *_shape;
	/*The actual body object*/
	btRigidBody *_body;
	/*Body's motion state*/
	btMotionState *_motionState;
	
	/*Local rotation of the body*/
	btQuaternion _localRot;
	/*World transform of the body*/
	btTransform _trans;
	/*Inverse worls transform of the body*/
	btTransform _transInv;
	/*Body's kinematic motion state*/
	btMotionState *_kinematicMotionState;
	/*Pointer to the Bullet world*/
	btDiscreteDynamicsWorld *_world;

public:
	AMRigidBody();
	~AMRigidBody();

	/*
		RigidBodyInfo* getInfo()

		Get a pointer to the rigid body's info structure

		Returns:		Pointer to the rigid body's info structure
	*/
	RigidBodyInfo* getInfo();
	/*
		bool init()

		Initialize the rigid body

		Parameters:		world			Pointer to the Bullet world
		Returns:		bool			Initialization completed sucessfully
	*/
	bool init(btDiscreteDynamicsWorld* world);
	/*
		void addToWorld()

		Add the rigid body to the world
	*/
	void addToWorld();
	/*
		void applyTransformToBone()

		Apply the transform of the body to its related bone
	*/
	void applyTransformToBone();
	/*
		btRigidBody* getBody()

		Get a pointer to the rigid body

		Returns:		Pointer to the rigid body
	*/
	btRigidBody* getBody();
	/*
		void setEnabled()

		Enable/disable the rigid body

		Parameters:		e				true=enabled,false=disabled
	*/
	void setEnabled(bool e);
	/*
		void updatePosition()

		Transform the body to match that of its related bone, resetting all kinematics
	*/
	void updatePosition();
};

/*
	Aligned motion state (from MMDAgent code, still not entirely sure what this does yet)
*/
class AlignedMotionState : public btMotionState
{
private:
	AMBone* _bone;
	btTransform _boneTrans;
	btTransform _boneTransInv;
	btTransform _graphicsWorldTrans;

public:
	AlignedMotionState(const btTransform &startTrans, const btTransform &boneTrans, AMBone* bone);
	virtual ~AlignedMotionState();

	virtual void getWorldTransform(btTransform &worldTrans) const;
	virtual void setWorldTransform(const btTransform &worldTrans);
	void setWorldTransformDirectly(const btTransform &worldTrans);
};

/*
	Kinematic motion state for kinematic bodies that just follow bones
*/
class KinematicMotionState : public btMotionState
{
private:
   AMBone *_bone;
   btTransform _boneTrans;

public:

   KinematicMotionState(const btTransform &startTrans, const btTransform &boneTrans, AMBone *bone);
   virtual ~KinematicMotionState();

   virtual void getWorldTransform(btTransform &worldTrans) const;
   virtual void setWorldTransform(const btTransform &worldTrans);
};

#endif