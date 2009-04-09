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
#include "Skeleton.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <bvh.h>
#include <MoteurGraphique.h>

#include <vector>
#include <ext/hash_map>

using namespace std;

class RagDoll : public Objet3D {
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

	RagDoll(const string & name, const SkeletonMesh * bvhFileName, const Material & mat, const Transform & transform, btDiscreteDynamicsWorld * m_ownerWorld, Monde3D * monde3D);

	virtual	~RagDoll();

	// affichage
	virtual void display() const;
	// fonction de mise a jour du personnage
	virtual void update(f32 elapsed);

private :

	static const Quaternion QUATERNION_UNIT_Y;
	static const Quaternion QUATERNION_MOINS_UNIT_Y;

	// constante qui definissent l'ensemble des contraintes d'une ragDoll
	static const int CONTRAINTES_BODY[JOINT_COUNT][2];
	static const int BODY_ALIGNE[BODYPART_COUNT][4];
	static const btQuaternion CONTRAINTES_ORIENTATIONS[JOINT_COUNT][2];
	static const btVector3 CONTRAINTES_LIMITES_ANGLES[JOINT_COUNT][2];
	static const bool CONTRAINTES_IS_CONE[JOINT_COUNT];

	btRigidBody * localCreateRigidBody(btScalar mass, f32 hauteur, f32 rayon, const Transform & globalTransformPart, int bodyPart, const Material & mat);
	static Quaternion getOrientationOs(const Vector3 &);
	Quaternion getOrientationOs(int part);

	const string name;
	// bvh
	const SkeletonMesh * skeletonMesh;
	Skeleton * skeleton;
	string skeletonName;
	int bodyIndex[BODYPART_COUNT];
	int rapportHauteursRayons[BODYPART_COUNT];
	f32 poids[BODYPART_COUNT];
	f32 scale;
	// physique
	btCapsuleShape * m_shapes[BODYPART_COUNT];
	btRigidBody * m_bodies[BODYPART_COUNT];
	btTypedConstraint * m_joints[JOINT_COUNT];
	btGeneric6DofConstraint * m_jointsGeneric6[JOINT_COUNT];
	btHingeConstraint * m_jointsHinge[JOINT_COUNT];
	btDiscreteDynamicsWorld * m_ownerWorld;
	// graphique
	Mesh * meshes[BODYPART_COUNT];
	Objet3D * objet3Ds[BODYPART_COUNT];
	Monde3D * monde3D;
	// animation
	f32 temps;
	bool sens;
};

#endif /* RAGDOLL_H_ */
