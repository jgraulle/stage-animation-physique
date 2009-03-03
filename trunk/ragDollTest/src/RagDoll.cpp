/*
 * RagDoll.cpp
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 *
 *  largement inspire de l'exemple de ragDoll fournit dans bullet
 */

#include "RagDoll.h"

const f32 RagDoll::TAILLE[RagDoll::BODYPART_COUNT][2] =
{
		{0.15, 0.20},
		{0.15, 0.28},
		{0.10, 0.05},
		{0.07, 0.45},
		{0.05, 0.37},
		{0.07, 0.45},
		{0.05, 0.37},
		{0.05, 0.33},
		{0.04, 0.25},
		{0.05, 0.33},
		{0.04, 0.25}
};

const Vector3 RagDoll::POSITIONS[RagDoll::BODYPART_COUNT] = {
	Vector3(0.0f, 1.0f, 0.0f),
	Vector3(0.0f, 1.2f, 0.f),
	Vector3(0.0f, 1.6f, 0.0f),
	Vector3(-0.18f, 0.65f, 0.0f),
	Vector3(-0.18f, 0.2f, 0.f),
	Vector3(0.18f, 0.65f, 0.0f),
	Vector3(0.18f, 0.2f, 0.0f),
	Vector3(-0.35f, 1.45f, 0.0f),
	Vector3(-0.7f, 1.45f, 0.0f),
	Vector3(0.35f, 1.45f, 0.0f),
	Vector3(0.7f, 1.45f, 0.0f)
};

const Quaternion RagDoll::ROTATIONS[RagDoll::BODYPART_COUNT] =
{
	Quaternion::IDENTITY,
	Quaternion::IDENTITY,
	Quaternion::IDENTITY,
	Quaternion::IDENTITY,
	Quaternion::IDENTITY,
	Quaternion::IDENTITY,
	Quaternion::IDENTITY,
	Quaternion(M_PI_2, Vector3::UNIT_Z),
	Quaternion(M_PI_2, Vector3::UNIT_Z),
	Quaternion(-M_PI_2, Vector3::UNIT_Z),
	Quaternion(-M_PI_2, Vector3::UNIT_Z)
};

const int RagDoll::CONTRAINTES_BODY[RagDoll::JOINT_COUNT][2] = {
	{BODYPART_PELVIS, BODYPART_SPINE},
	{BODYPART_SPINE, BODYPART_HEAD},
	{BODYPART_PELVIS, BODYPART_LEFT_UPPER_LEG},
	{BODYPART_LEFT_UPPER_LEG, BODYPART_LEFT_LOWER_LEG},
	{BODYPART_PELVIS, BODYPART_RIGHT_UPPER_LEG},
	{BODYPART_RIGHT_UPPER_LEG, BODYPART_RIGHT_LOWER_LEG},
	{BODYPART_SPINE, BODYPART_LEFT_UPPER_ARM},
	{BODYPART_LEFT_UPPER_ARM, BODYPART_LEFT_LOWER_ARM},
	{BODYPART_SPINE, BODYPART_RIGHT_UPPER_ARM},
	{BODYPART_RIGHT_UPPER_ARM, BODYPART_RIGHT_LOWER_ARM}
};

const btTransform RagDoll::CONTRAINTES_POSITIONS[RagDoll::JOINT_COUNT][2] = {
	{btTransform(btQuaternion(0,M_PI_2,0), btVector3(0.0f, 0.15f, 0.0f)), btTransform(btQuaternion(0,M_PI_2,0), btVector3(0.0f, -0.15f, 0.0f))},
	{btTransform(btQuaternion(0,0,M_PI_2), btVector3(0.0f, 0.30f, 0.0f)), btTransform(btQuaternion(0,0,M_PI_2), btVector3(0.0f, -0.14f, 0.0f))},
	{btTransform(btQuaternion(0,0,-M_PI_4*5), btVector3(-0.18f, -0.10f, 0.0f)), btTransform(btQuaternion(0,0,-M_PI_4*5), btVector3(0.0f, 0.225f, 0.0f))},
	{btTransform(btQuaternion(0,M_PI_2,0), btVector3(0.0f, -0.225f, 0.0f)), btTransform(btQuaternion(0,M_PI_2,0), btVector3(0.0f, 0.185f, 0.0f))},
	{btTransform(btQuaternion(0,0,M_PI_4), btVector3(0.18f, -0.10f, 0.0f)), btTransform(btQuaternion(0,0,M_PI_4), btVector3(0.0f, 0.225f, 0.0f))},
	{btTransform(btQuaternion(0,M_PI_2,0), btVector3(0.0f, -0.225f, 0.0f)), btTransform(btQuaternion(0,M_PI_2,0), btVector3(0.0f, 0.185f, 0.0f))},
	{btTransform(btQuaternion(0,0,M_PI), btVector3(-0.2f, 0.15f, 0.0f)), btTransform(btQuaternion(0,0,M_PI_2), btVector3(0.0f, -0.18f, 0.0f))},
	{btTransform(btQuaternion(0,M_PI_2,0), btVector3(0.0f, 0.18f, 0.0f)), btTransform(btQuaternion(0,M_PI_2,0), btVector3(0.0f, -0.14f, 0.0f))},
	{btTransform(btQuaternion(0,0,0), btVector3(0.2f, 0.15f, 0.0f)), btTransform(btQuaternion(0,0,M_PI_2), btVector3(0.0f, -0.18f, 0.0f))},
	{btTransform(btQuaternion(0,M_PI_2,0), btVector3(0.0f, 0.18f, 0.0f)), btTransform(btQuaternion(0,M_PI_2,0), btVector3(0.0f, -0.14f, 0.0f))}
};

const f32 RagDoll::CONTRAINTES_ANGLES[RagDoll::JOINT_COUNT][3] = {
	{-M_PI_4, M_PI_2, 0.0f},
	{M_PI_4, M_PI_4, M_PI_2},
	{M_PI_4, M_PI_4, 0},
	{0.0f, M_PI_2, 0.0f},
	{M_PI_4, M_PI_4, 0.0f},
	{0.0f, M_PI_2, 0.0f},
	{M_PI_2, M_PI_2, 0.0f},
	{-M_PI_2, 0.0f, 0.0f},
	{M_PI_2, M_PI_2, 0},
	{-M_PI_2, 0.0f, 0.0f}
};

const bool RagDoll::CONTRAINTES_IS_CONE[RagDoll::JOINT_COUNT] = {
	false,
	true,
	true,
	false,
	true,
	false,
	true,
	false,
	true,
	false
};

RagDoll::RagDoll (const string & name, const Material & mat, Transform transform, btDiscreteDynamicsWorld * m_ownerWorld, Monde3D * monde3D)
: name(name), m_ownerWorld(m_ownerWorld), monde3D(monde3D) {
	// creation des objets graphiques et physiques
	for (int part=0; part<BODYPART_COUNT; part++)
		m_bodies[part] = localCreateRigidBody(1.0f, transform, part, mat);

	// Now setup the constraints
	for (int joint=0; joint<JOINT_COUNT; joint++) {
		if (CONTRAINTES_IS_CONE[joint]) {
			m_joints[joint] = new btConeTwistConstraint(*m_bodies[CONTRAINTES_BODY[joint][0]], *m_bodies[CONTRAINTES_BODY[joint][1]], CONTRAINTES_POSITIONS[joint][0], CONTRAINTES_POSITIONS[joint][1]);
			((btConeTwistConstraint*)m_joints[joint])->setLimit(CONTRAINTES_ANGLES[joint][0], CONTRAINTES_ANGLES[joint][1], CONTRAINTES_ANGLES[joint][2]);
			m_ownerWorld->addConstraint(m_joints[joint], true);
		} else {
			m_joints[joint] = new btHingeConstraint(*m_bodies[CONTRAINTES_BODY[joint][0]], *m_bodies[CONTRAINTES_BODY[joint][1]], CONTRAINTES_POSITIONS[joint][0], CONTRAINTES_POSITIONS[joint][1]);
			((btHingeConstraint*)m_joints[joint])->setLimit(CONTRAINTES_ANGLES[joint][0], CONTRAINTES_ANGLES[joint][1]);
			m_ownerWorld->addConstraint(m_joints[joint], true);
		}
	}
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

btRigidBody * RagDoll::localCreateRigidBody(btScalar mass, const Transform & transform, int bodyPart, const Material & mat) {
	// position de la partie
	Transform startTransform = transform;
	startTransform.translate(POSITIONS[bodyPart]);
	startTransform.rotate(ROTATIONS[bodyPart]);

	// forme graphique
	meshes[bodyPart] = new Capsule(TAILLE[bodyPart][0], TAILLE[bodyPart][1], 8, 4);
	objet3Ds[bodyPart] = new Objet3D(mat, meshes[bodyPart], startTransform);
	stringstream tampon;
	tampon << name << bodyPart;
	monde3D->add(tampon.str(), objet3Ds[bodyPart]);

	// forme physique
	m_shapes[bodyPart] = new btCapsuleShape(TAILLE[bodyPart][0], TAILLE[bodyPart][1]);
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		m_shapes[bodyPart]->calculateLocalInertia(mass,localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, new TransformConv(&(objet3Ds[bodyPart]->getTransform())), m_shapes[bodyPart], localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	m_ownerWorld->addRigidBody(body);

	body->setDamping(0.05, 0.85);
	// body->setDeactivationTime(0.8);
	body->setActivationState(DISABLE_DEACTIVATION);
	body->setSleepingThresholds(1.6, 2.5);

	return body;
}

// fonction de mise a jour du personnage
void RagDoll::update(f32 elapsed) {
	// TODO
}
