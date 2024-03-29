/*
 * RagDoll.h
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 *
 *  largement inspire de l'exemple de ragDoll fournit dans bullet
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

	RagDoll(const string & name, const Material & mat, Transform transform, btDiscreteDynamicsWorld * m_ownerWorld, Monde3D * monde3D);

	virtual	~RagDoll();

	// fonction de mise a jour du personnage
	virtual void update(f32 elapsed);

private :

	static const f32 TAILLE[BODYPART_COUNT][2];
	static const Vector3 POSITIONS[BODYPART_COUNT];
	static const Quaternion ROTATIONS[BODYPART_COUNT];
	static const btTransform CONTRAINTES_POSITIONS[JOINT_COUNT][2];
	static const int CONTRAINTES_BODY[JOINT_COUNT][2];
	static const f32 CONTRAINTES_ANGLES[JOINT_COUNT][3];
	static const bool CONTRAINTES_IS_CONE[JOINT_COUNT];
	btRigidBody * localCreateRigidBody(btScalar mass, const Transform & transform, int bodyPart, const Material & mat);

	const string name;
	// physique
	btCollisionShape * m_shapes[BODYPART_COUNT];
	btRigidBody * m_bodies[BODYPART_COUNT];
	btTypedConstraint * m_joints[JOINT_COUNT];
	btDiscreteDynamicsWorld * m_ownerWorld;
	// graphique
	Mesh * meshes[BODYPART_COUNT];
	Objet3D * objet3Ds[BODYPART_COUNT];
	Monde3D * monde3D;
};

#endif /* RAGDOLL_H_ */
