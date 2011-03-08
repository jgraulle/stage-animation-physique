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
#include <list>
#include <ext/hash_map>
#include <fstream>

using namespace std;

class RagDoll : public Objet3D {
public :
	enum BodyPart {
		BODYPART_PELVIS = 0,
		BODYPART_SPINE = 1,
		BODYPART_HEAD = 2,

		BODYPART_LEFT_UPPER_LEG = 3,
		BODYPART_LEFT_LOWER_LEG = 4,

		BODYPART_RIGHT_UPPER_LEG = 5,
		BODYPART_RIGHT_LOWER_LEG = 6,

		BODYPART_LEFT_UPPER_ARM = 7,
		BODYPART_LEFT_LOWER_ARM = 8,

		BODYPART_RIGHT_UPPER_ARM = 9,
		BODYPART_RIGHT_LOWER_ARM = 10,

		BODYPART_COUNT
	};

	static const int BODYPART_LEFT_TO_RIGHT = 2;
	static const int BODYPART_RIGHT_TO_LEFT = -2;

	enum Joint {
		JOINT_PELVIS_SPINE = 0,
		JOINT_SPINE_HEAD = 1,

		JOINT_LEFT_HIP = 2,
		JOINT_LEFT_KNEE = 3,

		JOINT_RIGHT_HIP = 4,
		JOINT_RIGHT_KNEE = 5,

		JOINT_LEFT_SHOULDER = 6,
		JOINT_LEFT_ELBOW = 7,

		JOINT_RIGHT_SHOULDER = 8,
		JOINT_RIGHT_ELBOW = 9,

		JOINT_COUNT
	};

	static const Quaternion QUATERNION_UNIT_Y;
	static const Quaternion QUATERNION_MOINS_UNIT_Y;

	// constante qui definissent l'ensemble des contraintes d'une ragDoll
	static const int JOINT_TO_BODY[JOINT_COUNT][2];
	static const int BODY_ALIGNE[BODYPART_COUNT][4];
	static const btQuaternion JOINT_ORIENTATIONS[JOINT_COUNT][2];
	enum JointLimitesAngles {JOINT_LIMITES_ANGLES_BASSE=0, JOINT_LIMITES_ANGLES_HAUTE=1};
	static const btVector3 JOINT_LIMITES_ANGLES[JOINT_COUNT][2];
	static const bool JOINT_IS_CONE[JOINT_COUNT];

	enum Hierarchie {PERE = 0, FILS = 1};

	// parcours de la hierarchie des membres
	static BodyPart jointToBodyPart(Joint joint, Hierarchie hierarchie);
	static const list<Joint> & bodyPartToJointFils(BodyPart part);
	static Joint bodyPartToJointPere(BodyPart part);
	static BodyPart getBodyPartPere(BodyPart part);
	static const list<BodyPart> & getBodyPartFils(BodyPart part);

	// constructeur
	// si fileNameExportJoint="" par de sauvegarde des valeurs des angles des articulations
	RagDoll(const string & name, const SkeletonMesh * bvhFileName, const Material & mat, const Transform & transform, btDiscreteDynamicsWorld * m_ownerWorld, Monde3D * monde3D, const string & fileNameExportJoint = "");

	// destructeur
	virtual	~RagDoll();

	// affichage
	virtual void display() const;
	// fonction de mise a jour du personnage
	virtual void update(f32 elapsed);

	// modifier l'animation
	virtual void setBorne(int numFrameDebut, int numFrameFin) {skeleton->setBorne(numFrameDebut, numFrameFin);}
	virtual void setVitesse(f32 vitesse) {skeleton->setVitesse(vitesse);}
	virtual int getNumFrame() const {return skeleton->getNumFrame();}
	virtual void setNumFrame(int numFrame) {skeleton->setNumFrame(numFrame);}

	// acceder a la position courante d'un membre (son centre et ses extremites)
	virtual Transform getLocalTransformPart(BodyPart part);
	virtual Transform getLocalTransformPart(BodyPart part, SkeletonMesh::Extremite extremite);

private :
	// outils de creations
	btRigidBody * localCreateRigidBody(const Transform & globalTransformPart, int bodyPart, const Material & mat);

	// outils d'orientation des os
	static Quaternion getOrientation(const Vector3 &);
	Quaternion getOrientationOs(int part, int numFrame);

	// parcours de la hierarchie des membres
	static bool initBodyPartToJoint();
	static list<Joint> bodyPartToJoint[BODYPART_COUNT][2];
	static bool initBodyPartHierarchie();
	static list<BodyPart> bodyPartHierarchie[BODYPART_COUNT];

	const string name;
	Transform inverLocalToGlobal;
	// bvh
	const SkeletonMesh * skeletonMesh;
	Skeleton * skeleton;
	string skeletonName;
	int bodyIndexRagDollToSkeleton[BODYPART_COUNT];
	f32 rapportHauteursRayons[BODYPART_COUNT];
	f32 bodyPartPoids[BODYPART_COUNT];
	f32 bodyPartHauteurs[BODYPART_COUNT];
	f32 scale;
	// physique
	btCollisionShape * m_shapes[BODYPART_COUNT];
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

	// fichier de sauvergarde des angles de l'asservissement
	string fileNameExportJoint;
	ofstream fileExportJoint[JOINT_COUNT];
};

#endif /* RAGDOLL_H_ */
