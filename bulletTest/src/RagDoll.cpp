/*
 * RagDoll.cpp
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 *
 *  largement inspire de l'exemple de ragDoll fournit dans bullet
 */

#include "RagDoll.h"

RagDoll::RagDoll (const string & name, Monde3D * monde3D, btDynamicsWorld* ownerWorld, const Material & mat, const btVector3& positionOffset)
: m_ownerWorld (ownerWorld), monde3D(monde3D), name(name) {
	// Setup the geometry
	m_shapes[BODYPART_PELVIS] = new btCapsuleShape(btScalar(0.15), btScalar(0.20));
	meshes[BODYPART_PELVIS] = new Capsule("BODYPART_PELVIS",0.15, 0.20, 8, 4);
	m_shapes[BODYPART_SPINE] = new btCapsuleShape(btScalar(0.15), btScalar(0.28));
	meshes[BODYPART_SPINE] = new Capsule("BODYPART_SPINE",0.15, 0.28, 8, 4);
	m_shapes[BODYPART_HEAD] = new btCapsuleShape(btScalar(0.10), btScalar(0.05));
	meshes[BODYPART_HEAD] = new Capsule("BODYPART_HEAD",0.10, 0.05, 8, 4);
	m_shapes[BODYPART_LEFT_UPPER_LEG] = new btCapsuleShape(btScalar(0.07), btScalar(0.45));
	meshes[BODYPART_LEFT_UPPER_LEG] = new Capsule("BODYPART_LEFT_UPPER_LEG",0.07, 0.45, 8, 4);
	m_shapes[BODYPART_LEFT_LOWER_LEG] = new btCapsuleShape(btScalar(0.05), btScalar(0.37));
	meshes[BODYPART_LEFT_LOWER_LEG] = new Capsule("BODYPART_LEFT_LOWER_LEG",0.05, 0.37, 8, 4);
	m_shapes[BODYPART_RIGHT_UPPER_LEG] = new btCapsuleShape(btScalar(0.07), btScalar(0.45));
	meshes[BODYPART_RIGHT_UPPER_LEG] = new Capsule("BODYPART_RIGHT_UPPER_LEG",0.07, 0.45, 8, 4);
	m_shapes[BODYPART_RIGHT_LOWER_LEG] = new btCapsuleShape(btScalar(0.05), btScalar(0.37));
	meshes[BODYPART_RIGHT_LOWER_LEG] = new Capsule("BODYPART_RIGHT_LOWER_LEG",0.05, 0.37, 8, 4);
	m_shapes[BODYPART_LEFT_UPPER_ARM] = new btCapsuleShape(btScalar(0.05), btScalar(0.33));
	meshes[BODYPART_LEFT_UPPER_ARM] = new Capsule("BODYPART_LEFT_UPPER_ARM",0.05, 0.33, 8, 4);
	m_shapes[BODYPART_LEFT_LOWER_ARM] = new btCapsuleShape(btScalar(0.04), btScalar(0.25));
	meshes[BODYPART_LEFT_LOWER_ARM] = new Capsule("BODYPART_LEFT_LOWER_ARM",0.04, 0.25, 8, 4);
	m_shapes[BODYPART_RIGHT_UPPER_ARM] = new btCapsuleShape(btScalar(0.05), btScalar(0.33));
	meshes[BODYPART_RIGHT_UPPER_ARM] = new Capsule("BODYPART_RIGHT_UPPER_ARM",0.05, 0.33, 8, 4);
	m_shapes[BODYPART_RIGHT_LOWER_ARM] = new btCapsuleShape(btScalar(0.04), btScalar(0.25));
	meshes[BODYPART_RIGHT_LOWER_ARM] = new Capsule("BODYPART_RIGHT_LOWER_ARM",0.04, 0.25, 8, 4);

	// Setup all the rigid bodies
	btTransform offset; offset.setIdentity();
	offset.setOrigin(positionOffset);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.), btScalar(1.), btScalar(0.)));
	m_bodies[BODYPART_PELVIS] = localCreateRigidBody(btScalar(0.), offset*transform, BODYPART_PELVIS, mat);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.), btScalar(1.2), btScalar(0.)));
	m_bodies[BODYPART_SPINE] = localCreateRigidBody(btScalar(1.), offset*transform, BODYPART_SPINE, mat);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.), btScalar(1.6), btScalar(0.)));
	m_bodies[BODYPART_HEAD] = localCreateRigidBody(btScalar(1.), offset*transform,BODYPART_HEAD, mat);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-0.18), btScalar(0.65), btScalar(0.)));
	m_bodies[BODYPART_LEFT_UPPER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform,BODYPART_LEFT_UPPER_LEG, mat);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-0.18), btScalar(0.2), btScalar(0.)));
	m_bodies[BODYPART_LEFT_LOWER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, BODYPART_LEFT_LOWER_LEG, mat);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.18), btScalar(0.65), btScalar(0.)));
	m_bodies[BODYPART_RIGHT_UPPER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, BODYPART_RIGHT_UPPER_LEG, mat);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.18), btScalar(0.2), btScalar(0.)));
	m_bodies[BODYPART_RIGHT_LOWER_LEG] = localCreateRigidBody(btScalar(1.), offset*transform, BODYPART_RIGHT_LOWER_LEG, mat);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-0.35), btScalar(1.45), btScalar(0.)));
	transform.getBasis().setEulerZYX(0,0,M_PI_2);
	m_bodies[BODYPART_LEFT_UPPER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, BODYPART_LEFT_UPPER_ARM, mat);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(-0.7), btScalar(1.45), btScalar(0.)));
	transform.getBasis().setEulerZYX(0,0,M_PI_2);
	m_bodies[BODYPART_LEFT_LOWER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, BODYPART_LEFT_LOWER_ARM, mat);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.35), btScalar(1.45), btScalar(0.)));
	transform.getBasis().setEulerZYX(0,0,-M_PI_2);
	m_bodies[BODYPART_RIGHT_UPPER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, BODYPART_RIGHT_UPPER_ARM, mat);

	transform.setIdentity();
	transform.setOrigin(btVector3(btScalar(0.7), btScalar(1.45), btScalar(0.)));
	transform.getBasis().setEulerZYX(0,0,-M_PI_2);
	m_bodies[BODYPART_RIGHT_LOWER_ARM] = localCreateRigidBody(btScalar(1.), offset*transform, BODYPART_RIGHT_LOWER_ARM, mat);

	// Setup some damping on the m_bodies
	for (int i = 0; i < BODYPART_COUNT; ++i)
	{
		m_bodies[i]->setDamping(0.05, 0.85);
		m_bodies[i]->setDeactivationTime(0.8);
		m_bodies[i]->setSleepingThresholds(1.6, 2.5);
	}

	// Now setup the constraints
	btHingeConstraint* hingeC;
	btConeTwistConstraint* coneC;

	btTransform localA, localB;

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.15), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.15), btScalar(0.)));
	hingeC =  new btHingeConstraint(*m_bodies[BODYPART_PELVIS], *m_bodies[BODYPART_SPINE], localA, localB);
	hingeC->setLimit(btScalar(-M_PI_4), btScalar(M_PI_2));
	m_joints[JOINT_PELVIS_SPINE] = hingeC;
	m_ownerWorld->addConstraint(m_joints[JOINT_PELVIS_SPINE], true);


	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,M_PI_2); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.30), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_HEAD], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, M_PI_2);
	m_joints[JOINT_SPINE_HEAD] = coneC;
	m_ownerWorld->addConstraint(m_joints[JOINT_SPINE_HEAD], true);


	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,-M_PI_4*5); localA.setOrigin(btVector3(btScalar(-0.18), btScalar(-0.10), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,-M_PI_4*5); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.225), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_bodies[BODYPART_PELVIS], *m_bodies[BODYPART_LEFT_UPPER_LEG], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, 0);
	m_joints[JOINT_LEFT_HIP] = coneC;
	m_ownerWorld->addConstraint(m_joints[JOINT_LEFT_HIP], true);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(-0.225), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.185), btScalar(0.)));
	hingeC =  new btHingeConstraint(*m_bodies[BODYPART_LEFT_UPPER_LEG], *m_bodies[BODYPART_LEFT_LOWER_LEG], localA, localB);
	hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
	m_joints[JOINT_LEFT_KNEE] = hingeC;
	m_ownerWorld->addConstraint(m_joints[JOINT_LEFT_KNEE], true);


	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,M_PI_4); localA.setOrigin(btVector3(btScalar(0.18), btScalar(-0.10), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,M_PI_4); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.225), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_bodies[BODYPART_PELVIS], *m_bodies[BODYPART_RIGHT_UPPER_LEG], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, 0);
	m_joints[JOINT_RIGHT_HIP] = coneC;
	m_ownerWorld->addConstraint(m_joints[JOINT_RIGHT_HIP], true);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(-0.225), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(0.185), btScalar(0.)));
	hingeC =  new btHingeConstraint(*m_bodies[BODYPART_RIGHT_UPPER_LEG], *m_bodies[BODYPART_RIGHT_LOWER_LEG], localA, localB);
	hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
	m_joints[JOINT_RIGHT_KNEE] = hingeC;
	m_ownerWorld->addConstraint(m_joints[JOINT_RIGHT_KNEE], true);

//		m_bodies[BODYPART_RIGHT_LOWER_LEG]->setLinearVelocity(btVector3(0.0, 0.0, 10.0));
//		m_bodies[BODYPART_RIGHT_LOWER_LEG]->setLinearVelocity(btVector3(0.0, 0.0, -10.0));
	m_bodies[BODYPART_RIGHT_LOWER_LEG]->applyForce(btVector3(0.0, 0.0, 30.0), btVector3(0.0, 0.2, 0.0));
	m_bodies[BODYPART_LEFT_LOWER_LEG]->applyImpulse(btVector3(0.0, 0.0, 30.0), btVector3(0.0, 0.2, 0.0));

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,M_PI); localA.setOrigin(btVector3(btScalar(-0.2), btScalar(0.15), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.18), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_LEFT_UPPER_ARM], localA, localB);
	coneC->setLimit(M_PI_2, M_PI_2, 0);
	m_joints[JOINT_LEFT_SHOULDER] = coneC;
	m_ownerWorld->addConstraint(m_joints[JOINT_LEFT_SHOULDER], true);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.18), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
	hingeC =  new btHingeConstraint(*m_bodies[BODYPART_LEFT_UPPER_ARM], *m_bodies[BODYPART_LEFT_LOWER_ARM], localA, localB);
	hingeC->setLimit(btScalar(-M_PI_2), btScalar(0));
	m_joints[JOINT_LEFT_ELBOW] = hingeC;
	m_ownerWorld->addConstraint(m_joints[JOINT_LEFT_ELBOW], true);



	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,0,0); localA.setOrigin(btVector3(btScalar(0.2), btScalar(0.15), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.18), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_bodies[BODYPART_SPINE], *m_bodies[BODYPART_RIGHT_UPPER_ARM], localA, localB);
	coneC->setLimit(M_PI_2, M_PI_2, 0);
	m_joints[JOINT_RIGHT_SHOULDER] = coneC;
	m_ownerWorld->addConstraint(m_joints[JOINT_RIGHT_SHOULDER], true);

	localA.setIdentity(); localB.setIdentity();
	localA.getBasis().setEulerZYX(0,M_PI_2,0); localA.setOrigin(btVector3(btScalar(0.), btScalar(0.18), btScalar(0.)));
	localB.getBasis().setEulerZYX(0,M_PI_2,0); localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
	hingeC =  new btHingeConstraint(*m_bodies[BODYPART_RIGHT_UPPER_ARM], *m_bodies[BODYPART_RIGHT_LOWER_ARM], localA, localB);
	hingeC->setLimit(btScalar(-M_PI_2), btScalar(0));
	m_joints[JOINT_RIGHT_ELBOW] = hingeC;
	m_ownerWorld->addConstraint(m_joints[JOINT_RIGHT_ELBOW], true);
}

RagDoll::~RagDoll () {
	int i;

	// Remove all constraints
	for ( i = 0; i < JOINT_COUNT; ++i)
	{
		m_ownerWorld->removeConstraint(m_joints[i]);
		delete m_joints[i]; m_joints[i] = 0;
	}

	// Remove all bodies and shapes
	for ( i = 0; i < BODYPART_COUNT; ++i)
	{
		m_ownerWorld->removeRigidBody(m_bodies[i]);

		delete m_bodies[i]->getMotionState();

		delete m_bodies[i]; m_bodies[i] = 0;
		delete m_shapes[i]; m_shapes[i] = 0;
	}
}

void RagDoll::applyForce() {
	m_bodies[BODYPART_RIGHT_LOWER_LEG]->applyForce(btVector3(0.0, 0.0, 30.0), btVector3(0.0, 0.2, 0.0));
}

btRigidBody* RagDoll::localCreateRigidBody (btScalar mass, const btTransform& startTransform, int bodyPart, const Material & mat) {
	// forme graphique
	objet3Ds[bodyPart] = new Objet3D(mat, meshes[bodyPart], Transform());
	stringstream tampon;
	tampon << name << bodyPart;
	monde3D->add(tampon.str(), objet3Ds[bodyPart]);

	// forme physique
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		m_shapes[bodyPart]->calculateLocalInertia(mass,localInertia);

	TransformConv * myMotionState = new TransformConv(&(objet3Ds[bodyPart]->getTransform()));
	myMotionState->setWorldTransform(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,m_shapes[bodyPart],localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setActivationState(DISABLE_DEACTIVATION);

	m_ownerWorld->addRigidBody(body);

	return body;
}
