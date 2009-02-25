/*
 * RagDoll.h
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef RAGDOLL_H_
#define RAGDOLL_H_

#include "TransformConv.h"

#include <btBulletDynamicsCommon.h>
#include "../../moteurGraphique/src/MoteurGraphique.h"

class RagDoll {
public :
	enum {
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

	enum {
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

	RagDoll (const string & name, Monde3D * monde3D, btDynamicsWorld* ownerWorld, const Material & mat, const btVector3& positionOffset);

	virtual	~RagDoll ();

	void applyForce();

private :
	btRigidBody* localCreateRigidBody (btScalar mass, const btTransform& startTransform, int bodyPart, const Material & mat);

	btDynamicsWorld* m_ownerWorld;
	Monde3D * monde3D;
	btCollisionShape* m_shapes[BODYPART_COUNT];
	Mesh * meshes[BODYPART_COUNT];
	btRigidBody* m_bodies[BODYPART_COUNT];
	Objet3D* objet3Ds[BODYPART_COUNT];
	btTypedConstraint* m_joints[JOINT_COUNT];
	string name;
};

#endif /* RAGDOLL_H_ */
