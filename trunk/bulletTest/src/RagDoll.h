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
#include "Perso.h"

#include <btBulletDynamicsCommon.h>
#include <bvh.h>
#include <MoteurGraphique.h>

#include <vector>

using namespace std;

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

	struct Os {
		const Vector3 debut, fin;
		int joint;
		Os(const Vector3 & debut, const Vector3 & fin, int joint) : debut(debut), fin(fin), joint(joint) {}
	};

	RagDoll(const string & name, const string & bvhFileName, const Material & mat, const Transform & transform, const Quaternion & orientationEdition, btDiscreteDynamicsWorld * m_ownerWorld, Monde3D * monde3D);

	virtual	~RagDoll();

	// fonction de mise a jour du personnage
	virtual void update(f32 elapsed);

private :

	static const int INDEX_EXAMPLE1[BODYPART_COUNT];
	static const int INDEX_WALK[BODYPART_COUNT];
	static const int RAPPORT_HAUTEURS_RAYONS[BODYPART_COUNT];
	static const int CONTRAINTES_BODY[JOINT_COUNT][2];
	static const btQuaternion CONTRAINTES_ORIENTATIONS[JOINT_COUNT][2];
	static const f32 CONTRAINTES_LIMITES_ANGLES[JOINT_COUNT][3];
	static const bool CONTRAINTES_IS_CONE[JOINT_COUNT];
	static f32 tailles[BODYPART_COUNT][2];
	static Vector3 positions[BODYPART_COUNT];
	static Quaternion rotations[BODYPART_COUNT];
	static btTransform contraintesPositions[JOINT_COUNT][2];

	btRigidBody * localCreateRigidBody(btScalar mass, const Transform & transform, int bodyPart, const Material & mat);
	void bvhRecursif(int joinId, const Vector3 & accumulateur);

	const string name;
	// bvh
	const string bvhFileName;
	MOTION * motion;
	MFRAME * frame;
	vector<Os *> osList;
	Perso * perso;
	Quaternion orientationEdition;
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
