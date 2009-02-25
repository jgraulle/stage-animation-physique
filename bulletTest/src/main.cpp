/*
 * main.cpp
 *
 *  Created on: 12 fevr. 2009
 *      Author: jeremie
 */

#include <iostream>
#include <sstream>
#include <GL/glfw.h>
#include "../../moteurGraphique/src/MoteurGraphique.h"
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

#define SCALING 1.0

using namespace std;


class BulletTransform : public btMotionState {
public:
	BulletTransform(Transform * tranform) : tranform(tranform) {}
	virtual void getWorldTransform(btTransform & worldTrans) const {
		worldTrans.setOrigin(btVector3(tranform->getPosition().x*SCALING, tranform->getPosition().y*SCALING, tranform->getPosition().z*SCALING));
		worldTrans.setRotation(btQuaternion(tranform->getOrientation().x, tranform->getOrientation().y, tranform->getOrientation().z, tranform->getOrientation().w));
	}
	virtual void setWorldTransform(const btTransform & worldTrans) {
		tranform->setPosition(worldTrans.getOrigin().getX()/SCALING, worldTrans.getOrigin().getY()/SCALING, worldTrans.getOrigin().getZ()/SCALING);
		tranform->getOrientation().w = worldTrans.getRotation().getW();
		tranform->getOrientation().x = worldTrans.getRotation().getX();
		tranform->getOrientation().y = worldTrans.getRotation().getY();
		tranform->getOrientation().z = worldTrans.getRotation().getZ();
	}
private:
	Transform * tranform;
};

class DebugDraw : public btIDebugDraw {
	virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color) {
		Disable lumiere(GL_LIGHTING);
		Disable texture(GL_TEXTURE_2D);
		Disable profondeur(GL_DEPTH_TEST);
		glBegin(GL_LINES);
		glColor4f(color.getX(), color.getY(), color.getZ(),1.f);
		glVertex3d(from.getX()/SCALING, from.getY()/SCALING, from.getZ()/SCALING);
		glVertex3d(to.getX()/SCALING, to.getY()/SCALING, to.getZ()/SCALING);
		glEnd();
	}
	virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) {}
	virtual void reportErrorWarning(const char * warningString) {cout << warningString;}
	virtual void draw3dText(const btVector3& location,const char* textString) {cout << textString;}
	virtual void setDebugMode(int debugMode) {}
	virtual int	getDebugMode() const {return DBG_DrawWireframe;}
};

btRigidBody::btRigidBodyConstructionInfo createRigidBodyInfo(btScalar mass, Transform & transform, btCollisionShape* collisionShape) {
	btVector3 inertie;
	collisionShape->calculateLocalInertia(mass, inertie);
	return btRigidBody::btRigidBodyConstructionInfo(mass, (btMotionState*)new BulletTransform(&transform), collisionShape, inertie);
}

class RagDoll
{
	enum
	{
		BODYPART_PELVIS = 0,
		BODYPART_SPINE,
		BODYPART_HEAD,

		BODYPART_LEFT_UPPER_LEG,
		BODYPART_LEFT_LOWER_LEG,

		BODYPART_RIGHT_UPPER_LEG,
		BODYPART_RIGHT_LOWER_LEG,

		BODYPART_LEFT_UPPER_ARM,
		BODYPART_LEFT_LOWER_ARM,

		BODYPART_RIGHT_UPPER_ARM,
		BODYPART_RIGHT_LOWER_ARM,

		BODYPART_COUNT
	};

	enum
	{
		JOINT_PELVIS_SPINE = 0,
		JOINT_SPINE_HEAD,

		JOINT_LEFT_HIP,
		JOINT_LEFT_KNEE,

		JOINT_RIGHT_HIP,
		JOINT_RIGHT_KNEE,

		JOINT_LEFT_SHOULDER,
		JOINT_LEFT_ELBOW,

		JOINT_RIGHT_SHOULDER,
		JOINT_RIGHT_ELBOW,

		JOINT_COUNT
	};

	btDynamicsWorld* m_ownerWorld;
	Monde3D * monde3D;
	btCollisionShape* m_shapes[BODYPART_COUNT];
	Mesh * meshes[BODYPART_COUNT];
	btRigidBody* m_bodies[BODYPART_COUNT];
	Objet3D* objet3Ds[BODYPART_COUNT];
	btTypedConstraint* m_joints[JOINT_COUNT];
	string name;

	btRigidBody* localCreateRigidBody (btScalar mass, const btTransform& startTransform, int bodyPart, const Material & mat)
	{
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

		BulletTransform * myMotionState = new BulletTransform(&(objet3Ds[bodyPart]->getTransform()));
		myMotionState->setWorldTransform(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,m_shapes[bodyPart],localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
		body->setActivationState(DISABLE_DEACTIVATION);

		m_ownerWorld->addRigidBody(body);

		return body;
	}

public:
	RagDoll (const string & name, Monde3D * monde3D, btDynamicsWorld* ownerWorld, const Material & mat, const btVector3& positionOffset)
		: m_ownerWorld (ownerWorld), monde3D(monde3D), name(name)
	{
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

	virtual	~RagDoll ()
	{
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

	void applyForce() {
		m_bodies[BODYPART_RIGHT_LOWER_LEG]->applyForce(btVector3(0.0, 0.0, 30.0), btVector3(0.0, 0.2, 0.0));
	}
};

void ajouterPerso(const string & name, Vector3 positionPerso, Material & mat, Monde3D * monde3D, btDynamicsWorld * dynamicsWorld) {
	// ajout de la colonne
	Objet3D * colonneGraphique = new Objet3D(mat, "cube", Transform(positionPerso, Quaternion(), Vector3(0.1, 0.7, 0.1)));
	monde3D->add(name+"colonne", colonneGraphique);
	btRigidBody * colonnePhysique = new btRigidBody(createRigidBodyInfo(0.0, colonneGraphique->getTransform(), new btBoxShape(btVector3(0.05, 0.35, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(colonnePhysique);
	colonnePhysique->setActivationState(DISABLE_DEACTIVATION);

//	btPoint2PointConstraint * contrainteColonne = new btPoint2PointConstraint(*colonnePhysique, btVector3(0.0, 0.35, 0.0)*SCALING);
//	dynamicsWorld->addConstraint(contrainteColonne, true);

	// ajout du bras droit
	Objet3D * brasDroiteGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(0.5,0.35,0.0), Quaternion(), Vector3(0.4, 0.1, 0.1)));
	monde3D->add(name+"brasDroit", brasDroiteGraphique);
	btRigidBody * brasDroitePhysique = new btRigidBody(createRigidBodyInfo(0.8, brasDroiteGraphique->getTransform(), new btBoxShape(btVector3(0.2, 0.05, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(brasDroitePhysique);
	brasDroitePhysique->setActivationState(DISABLE_DEACTIVATION);

	btConeTwistConstraint * epauleDroiteContrainte = new btConeTwistConstraint(*colonnePhysique, *brasDroitePhysique, btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(0.25, 0.35, 0.0)*SCALING), btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(-0.2, 0.0, 0.0)*SCALING));
	epauleDroiteContrainte->setLimit(M_PI_2*0.9, M_PI_4, M_PI_4);
	dynamicsWorld->addConstraint(epauleDroiteContrainte, false);

	// ajout de l'avant bras droit
	Objet3D * avantBrasDroiteGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(0.5+0.4,0.35,0.0), Quaternion(), Vector3(0.4, 0.1, 0.1)));
	monde3D->add(name+"avantBrasDroit", avantBrasDroiteGraphique);
	btRigidBody * avantBrasDroitePhysique = new btRigidBody(createRigidBodyInfo(0.2, avantBrasDroiteGraphique->getTransform(), new btBoxShape(btVector3(0.2, 0.05, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(avantBrasDroitePhysique);
	avantBrasDroitePhysique->setActivationState(DISABLE_DEACTIVATION);

	btHingeConstraint * coudeDroiteContrainte = new btHingeConstraint(*brasDroitePhysique, *avantBrasDroitePhysique, btTransform(btQuaternion(0.0, M_PI_2, 0.0), btVector3(0.2, 0.0, 0.0)*SCALING), btTransform(btQuaternion(0.0, M_PI_2, 0.0), btVector3(-0.2, 0.0, 0.0)*SCALING));
	coudeDroiteContrainte->setLimit(0.0, M_PI_2+M_PI_4);	// on peut plier le coude au 3/4 en avant mais pas du tout en arriere
	dynamicsWorld->addConstraint(coudeDroiteContrainte, true);

	// ajout du bras gauche
	Objet3D * brasGaucheGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(-0.5,0.35,0.0), Quaternion(), Vector3(0.4, 0.1, 0.1)));
	monde3D->add(name+"brasGauche", brasGaucheGraphique);
	btRigidBody * brasGauchePhysique = new btRigidBody(createRigidBodyInfo(0.8, brasGaucheGraphique->getTransform(), new btBoxShape(btVector3(0.2, 0.05, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(brasGauchePhysique);
	brasGauchePhysique->setActivationState(DISABLE_DEACTIVATION);

	btConeTwistConstraint * epauleGaucheContrainte = new btConeTwistConstraint(*colonnePhysique, *brasGauchePhysique, btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(-0.25, 0.35, 0.0)*SCALING), btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(0.2, 0.0, 0.0)*SCALING));
	epauleGaucheContrainte->setLimit(M_PI_2*0.9, M_PI_4, M_PI_4);
	dynamicsWorld->addConstraint(epauleGaucheContrainte, false);

	// ajout de l'avant bras gauche
	Objet3D * avantBrasGaucheGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(-0.5-0.4,0.35,0.0), Quaternion(), Vector3(0.4, 0.1, 0.1)));
	monde3D->add(name+"avantGaucheDroit", avantBrasGaucheGraphique);
	btRigidBody * avantBrasGauchePhysique = new btRigidBody(createRigidBodyInfo(0.2, avantBrasGaucheGraphique->getTransform(), new btBoxShape(btVector3(0.2, 0.05, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(avantBrasGauchePhysique);
	avantBrasGauchePhysique->setActivationState(DISABLE_DEACTIVATION);

	btHingeConstraint * coudeGaucheContrainte = new btHingeConstraint(*brasGauchePhysique, *avantBrasGauchePhysique, btTransform(btQuaternion(0.0, M_PI_2, 0.0), btVector3(-0.2, 0.0, 0.0)*SCALING), btTransform(btQuaternion(0.0, M_PI_2, 0.0), btVector3(0.2, 0.0, 0.0)*SCALING));
	coudeGaucheContrainte->setLimit(-M_PI_2-M_PI_4, 0.0);	// on peut plier le coude au 3/4 en avant mais pas du tout en arriere
	dynamicsWorld->addConstraint(coudeGaucheContrainte, true);

	// ajout de la cuisse droit
	Objet3D * cuisseDroiteGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(0.25,-0.7,0.0), Quaternion(), Vector3(0.1, 0.5, 0.1)));
	monde3D->add(name+"cuisseDroite", cuisseDroiteGraphique);
	btRigidBody * cuisseDroitePhysique = new btRigidBody(createRigidBodyInfo(0.8, cuisseDroiteGraphique->getTransform(), new btBoxShape(btVector3(0.05, 0.25, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(cuisseDroitePhysique);
	cuisseDroitePhysique->setActivationState(DISABLE_DEACTIVATION);

	btConeTwistConstraint * bassinDroitContrainte = new btConeTwistConstraint(*colonnePhysique, *cuisseDroitePhysique, btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(0.25, -0.35, 0.0)*SCALING), btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(0.0, 0.25, 0.0)*SCALING));
	bassinDroitContrainte->setLimit(M_PI_4, M_PI_4, M_PI_4);
	dynamicsWorld->addConstraint(bassinDroitContrainte, true);

	// ajout du mollet droit
	Objet3D * molletDroitGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(0.25, -0.7-0.5, 0.0), Quaternion(), Vector3(0.1, 0.5, 0.1)));
	monde3D->add(name+"molletDroit", molletDroitGraphique);
	btRigidBody * molletDroitPhysique = new btRigidBody(createRigidBodyInfo(0.2, molletDroitGraphique->getTransform(), new btBoxShape(btVector3(0.05, 0.25, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(molletDroitPhysique);
	molletDroitPhysique->setActivationState(DISABLE_DEACTIVATION);

	btHingeConstraint * genouxDroitContrainte = new btHingeConstraint(*cuisseDroitePhysique, *molletDroitPhysique, btTransform(btQuaternion(M_PI_2, 0.0, 0.0), btVector3(0.0, -0.25, 0.0)*SCALING), btTransform(btQuaternion(M_PI_2, 0.0, 0.0), btVector3(0.0, 0.25, 0.0)*SCALING));
	genouxDroitContrainte->setLimit(0.0, M_PI_2+M_PI_4);	// on peut plier le genoux au 3/4 en arriere mais pas du tout en avant
	dynamicsWorld->addConstraint(genouxDroitContrainte, true);

	// ajout de la cuisse gauche
	Objet3D * cuisseGaucheGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(-0.25, -0.7, 0.0), Quaternion(), Vector3(0.1, 0.5, 0.1)));
	monde3D->add(name+"cuisseGauche", cuisseGaucheGraphique);
	btRigidBody * cuisseGauchePhysique = new btRigidBody(createRigidBodyInfo(0.8, cuisseGaucheGraphique->getTransform(), new btBoxShape(btVector3(0.05, 0.25, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(cuisseGauchePhysique);
	cuisseGauchePhysique->setActivationState(DISABLE_DEACTIVATION);

	btConeTwistConstraint * bassinGaucheContrainte = new btConeTwistConstraint(*colonnePhysique, *cuisseGauchePhysique, btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(-0.25, -0.35, 0.0)*SCALING), btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(0.0, 0.25, 0.0)*SCALING));
	bassinGaucheContrainte->setLimit(M_PI_4, M_PI_4, M_PI_4);
	dynamicsWorld->addConstraint(bassinGaucheContrainte, true);

	// ajout du mollet gauche
	Objet3D * molletGaucheGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(-0.25, -0.7-0.5, 0.0), Quaternion(), Vector3(0.1, 0.5, 0.1)));
	monde3D->add(name+"molletGauche", molletGaucheGraphique);
	btRigidBody * molletGauchePhysique = new btRigidBody(createRigidBodyInfo(0.2, molletGaucheGraphique->getTransform(), new btBoxShape(btVector3(0.05, 0.25, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(molletGauchePhysique);
	molletGauchePhysique->setActivationState(DISABLE_DEACTIVATION);

	btHingeConstraint * genouxGaucheContrainte = new btHingeConstraint(*cuisseGauchePhysique, *molletGauchePhysique, btTransform(btQuaternion(M_PI_2, 0.0, 0.0), btVector3(0.0, -0.25, 0.0)*SCALING), btTransform(btQuaternion(M_PI_2, 0.0, 0.0), btVector3(0.0, 0.25, 0.0)*SCALING));
	genouxGaucheContrainte->setLimit(0.0, M_PI_2+M_PI_4);	// on peut plier le genoux au 3/4 en arriere mais pas du tout en avant
	dynamicsWorld->addConstraint(genouxGaucheContrainte, true);

	molletGauchePhysique->setAngularVelocity(btVector3(2.5, 0.0, 2.5));
}

// gestion du drag and drop de la souris
void gestionSouris(Camera & camera) {
	// coordonn�e de la souris courrante
	int x, y;
	glfwGetMousePos(&x, &y);
	int cran = glfwGetMouseWheel();

	// variables conservant l'ancienne position de la souris
	static int sourisX=x, sourisY=y, cranAnc = cran;

	// si la souris a boug�
	if (sourisX!=x || sourisY!=y) {
		// si le bouton gauche est press�
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS) {
			camera.addRot((double)(x-sourisX), (double)(y-sourisY));
		}
		// si le bouton droit est press�
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS) {
			camera.moveCentreObser(Vector3((double)(x-sourisX)/10.0, 0.0, (double)(y-sourisY)/10.0));
		}
		// si le bouton du milieu est press�
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)==GLFW_PRESS) {
			camera.moveCentreObser(Vector3(0.0, (double)(sourisY-y)/10.0, 0.0));
		}
	}
	// si la molette de la souris a tourn�
	if (cranAnc != cran) {
		camera.addDistance((double)(cranAnc-cran)*2.0);
	}

	// si une des touche fleche du clavier est presse
	if (glfwGetKey(GLFW_KEY_UP)==GLFW_PRESS)
		camera.moveCentreObser(Vector3(0.0, 0.0, -0.02));
	if (glfwGetKey(GLFW_KEY_DOWN)==GLFW_PRESS)
		camera.moveCentreObser(Vector3(0.0, 0.0, 0.02));
	if (glfwGetKey(GLFW_KEY_LEFT)==GLFW_PRESS)
		camera.moveCentreObser(Vector3(-0.02, 0.0, 0.0));
	if (glfwGetKey(GLFW_KEY_RIGHT)==GLFW_PRESS)
		camera.moveCentreObser(Vector3(0.02, 0.0, 0.0));

	// mise a jour de l'ancienne position de la souris
	sourisX = x;
	sourisY = y;
	cranAnc = cran;
}

int main(int argc, char **argv) {
	try {
		// creation du monde
		Moteur * moteur = Moteur::getInstance();
		Monde3D * monde3D = moteur->getMonde3D();
		Monde2D * monde2D = moteur->getMonde2D();

		// camera
		Camera * camera = monde3D->getCamera();
		camera->setCentreObser(Vector3(0.0f, 0.0f, 0.0f));
		Quaternion direction = Quaternion::IDENTITY;
		camera->setDistance(20);
		camera->setRot(0.0, 45.0);

		// ajout des lumieres
		monde3D->add("lumiere0", new Lumiere(Vector3(10.0,10.0,10.0), Couleur::BLANC));
		monde3D->add("lumiere1", new Lumiere(Vector3(-10.0,10.0,-10.0), Couleur::BLANC));

		// chargement des ressources
		const Image * im = ImagesManager::getInstance()->add(new Image("data/cube.png"));
		TexturesManager::getInstance()->add(new Texture("data/cube.png", true));
		Material mat("data/cube.png");
		MeshManager::getInstance()->add(new Cube("cube"));
//		MeshManager::getInstance()->add(new Sphere("sphere", 16, 8));
		MeshManager::getInstance()->add(new Cylinder("cylindre", 16));
//		MeshManager::getInstance()->add(new Capsule("capsule", 2.0, 8.0, 16, 8));
/*		f32 positions[][3] = {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
		f32 normals[][3] = {{0.0f, 0.0f, 1.0f}};
		f32 texCoords[][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}};
		u16 faces[][3][3] = {{{0,1,2},{0,0,0},{0,1,2}}};
		MeshManager::getInstance()->add(new Mesh("testMesh", positions, normals, texCoords, faces, 1));
		monde3D->add("testMesh", new Objet3D(mat, "testMesh", Transform(Vector3(4.0,0.0,0.0))));
*/
		// creation du monde physique
		btDefaultCollisionConfiguration * collisionConfiguration = new btDefaultCollisionConfiguration();
		btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
		btAxisSweep3 * overlappingPairCache = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024);
		btSequentialImpulseConstraintSolver * solver = new btSequentialImpulseConstraintSolver;
		btDiscreteDynamicsWorld * dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
		DebugDraw * debugDrawer = new DebugDraw();
		dynamicsWorld->setDebugDrawer(debugDrawer);

		// ajout du sol graphique
		Quaternion quat = Quaternion::IDENTITY;
//		quat.FromAngleAxis(M_PI/2.0, Vector3::UNIT_X);
		Objet3D * solGraphique = new Objet3D(mat, "cube", Transform(Vector3(0.0,-5.0,0.0), quat, Vector3(50.0, 5.0, 50.0)));
		monde3D->add("sol", solGraphique);
		// ajout du sol physique
		btRigidBody * solPhysique = new btRigidBody(createRigidBodyInfo(0.0, solGraphique->getTransform(), new btBoxShape(btVector3(50.0, 5.0, 50.0)*SCALING)));
		dynamicsWorld->addRigidBody(solPhysique);

/*		// debug forme
		Objet3D * cylindreGraphique = new Objet3D(mat, "cylindre", Transform(Vector3(4.0,5.0,0.0), Quaternion(), Vector3(2.0, 2.0, 4.0)));
		monde3D->add("testCapsule", cylindreGraphique);
		btRigidBody * cylindrePhysique = new btRigidBody(createRigidBodyInfo(2.0, cylindreGraphique->getTransform(), new btCapsuleShapeZ(2.0*SCALING, 8.0*SCALING)));
		cylindrePhysique->setAngularVelocity(btVector3(2.0, 0.0, 2.0));
		dynamicsWorld->addRigidBody(cylindrePhysique);
*/

/*		// ajout de dominos
		for (float x=-30.0; x<10.0; x+=1.4) {
			char tampon[80];
			sprintf(tampon, "domino%2f%", x);
			// ajout d'un cube graphique
			Objet3D * cubeGraphique = new Objet3D(mat, "cube", Transform(Vector3(x,1.0,0), Quaternion(), Vector3(0.1, 1.0, 0.4)));
			monde3D->add(tampon, cubeGraphique);
			// ajout d'un cube physique
			btRigidBody * cubePhysique = new btRigidBody(createRigidBodyInfo(0.01, cubeGraphique->getTransform(), new btBoxShape(btVector3(0.1, 1.0, 0.4)*SCALING)));
			dynamicsWorld->addRigidBody(cubePhysique);
		}
		// ajout d'une sphere qui declanche les dominos
		// ajout d'une sphere graphique
		Objet3D * sphere = new Objet3D(mat, "sphere", Transform(Vector3(12.0, 1.0, 0.0), Quaternion(), Vector3(1.0, 1.0, 1.0)));
		monde3D->add("sphere", sphere);
		// ajout d'un sphere physique
		btRigidBody * spherePhysique = new btRigidBody(createRigidBodyInfo(0.02, sphere->getTransform(), new btSphereShape(1.0*SCALING)));
		spherePhysique->setLinearVelocity(btVector3(-5.0, 0.0, 0.0));
		dynamicsWorld->addRigidBody(spherePhysique);
*/
/*
		for (float y=-5.0; y<5.0; y+=2.0) {
			for (float x=-5.0; x<5.0; x+=2.0) {
				stringstream tampon;
				tampon << "RagDoll(" << x << "," << y << ")-";
				new RagDoll(tampon.str(), monde3D, dynamicsWorld, mat, btVector3(x, Random::getf32(2.90, 3.10), y));
				//ajouterPerso(tampon.str(), Vector3(x, Random::getf32(2.90, 3.10), y), mat, monde3D, dynamicsWorld);
			}
		}
*/

		RagDoll * toto = new RagDoll("coucou2", monde3D, dynamicsWorld, mat, btVector3(0.0, 3.0, 3.0));
		ajouterPerso("cocuou", Vector3(0.0, 3.0, 0.0), mat, monde3D, dynamicsWorld);

/*		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		Disable texture(GL_TEXTURE_2D);
		Disable lumiere(GL_LIGHTING);
*/
		// affichage du monde
		do {
			moteur->update();
			dynamicsWorld->stepSimulation(moteur->getElapsed()*1.0);
			gestionSouris(*camera);
			moteur->display();
			glLoadIdentity();
			moteur->getMonde3D()->getCamera()->positionner();
//			dynamicsWorld->debugDrawWorld();
			moteur->swapBuffer();
			toto->applyForce();
		} while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));

		delete dynamicsWorld;

	} catch (Erreur e) {
		cout << e.what() << endl;
	} catch (exception e) {
		cout << e.what() << endl;
	}
	return 0;
}
