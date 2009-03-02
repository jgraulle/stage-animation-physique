/*
 * RagDoll.cpp
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 *
 *  largement inspire de l'exemple de ragDoll fournit dans bullet
 */

#include "RagDoll.h"

f32 RagDoll::TAILLE[RagDoll::BODYPART_COUNT][2] =
{
		{0.15, 0.20}, // BODYPART_PELVIS
		{0.15, 0.28}, // BODYPART_SPINE
		{0.10, 0.05}, // BODYPART_HEAD
		{0.07, 0.45},
		{0.05, 0.37},
		{0.07, 0.45},
		{0.05, 0.37},
		{0.05, 0.33},
		{0.04, 0.25},
		{0.05, 0.33},
		{0.04, 0.25}
};

Vector3 RagDoll::POSITIONS[RagDoll::BODYPART_COUNT] = {
	Vector3(0.0f, 1.0f, 0.0f),		// BODYPART_PELVIS
	Vector3(0.0f, 1.2f, 0.f),		// BODYPART_SPINE
	Vector3(0.0f, 1.6f, 0.0f),		// BODYPART_HEAD
	Vector3(-0.18f, 0.65f, 0.0f),
	Vector3(-0.18f, 0.2f, 0.f),
	Vector3(0.18f, 0.65f, 0.0f),
	Vector3(0.18f, 0.2f, 0.0f),
	Vector3(-0.35f, 1.45f, 0.0f),
	Vector3(-0.7f, 1.45f, 0.0f),
	Vector3(0.35f, 1.45f, 0.0f),
	Vector3(0.7f, 1.45f, 0.0f)
};

const int RagDoll::INDEX_BVH[RagDoll::BODYPART_COUNT] = {
	11,	// BODYPART_PELVIS hanche
	12,	// BODYPART_SPINE corps haut
	14, // BODYPART_HEAD tete
	1,	// BODYPART_LEFT_UPPER_LEG jambe haut gauche
	2,	// BODYPART_LEFT_LOWER_LEG jambe bas gauche
	6,	// BODYPART_RIGHT_UPPER_LEG jambe haut droite
	7,	// BODYPART_RIGHT_LOWER_LEG jambe bas droite
	17,	// BODYPART_LEFT_UPPER_ARM bras haut gauche
	18,	// BODYPART_LEFT_LOWER_ARM bras bas gauche
	23,	// BODYPART_RIGHT_UPPER_ARM bras haut droite
	24	// BODYPART_RIGHT_LOWER_ARM bras bas droite
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

RagDoll::RagDoll (const string & name, const string & bvhFileName, const Material & mat, Transform transform, btDiscreteDynamicsWorld * m_ownerWorld, Monde3D * monde3D)
: name(name), bvhFileName(bvhFileName), m_ownerWorld(m_ownerWorld), monde3D(monde3D) {
	// chargement du fichier bvh
	if(motion_load_bvh(&motion, bvhFileName.c_str()) < 0)
		throw ErreurFileNotFound(bvhFileName, "");
	// initialisation de la structure frame
	frame = motion_frame_init(NULL, motion);
	// lecture du bvh en position d'edition
	motion_get_frame(frame, motion, 0);
	// acces a la position du root
	int rootId = motion_frame_get_root_joint(frame);
	if (rootId==-1)
		throw Erreur("le fichier '"+bvhFileName+"' n'a pas de root !");
	Vector3 offset;
	joint_get_offset(frame, rootId, offset);

	osList.resize(motion_frame_get_joints_n(frame), NULL);
	bvhRecursif(rootId, offset);
	vector<Os *>::iterator it = osList.begin();
	while (it!=osList.end()) {
		if (*it!=NULL)
			cout << (*it)->joint << " " << joint_get_name(frame, (*it)->joint) << " " << (*it)->debut << (*it)->fin << endl;
		it++;
	}

	f32 tailleBvh = (osList[3]->fin - osList[0]->debut).length();
	f32 tailleRagDoll = POSITIONS[BODYPART_HEAD].length()+TAILLE[BODYPART_HEAD][1];
	cout << "taille bvh " << tailleBvh << endl;
	cout << "taille ragdoll " << tailleRagDoll << endl;
	f32 scale = tailleBvh / tailleRagDoll;
	scale = 1.0f;
/* Example1.bvh
	// taille tete
	cout << "taille Head " << TAILLE[BODYPART_HEAD][1] << " ";
	TAILLE[BODYPART_HEAD][1] = (osList[3]->fin - osList[3]->debut).length()/scale;
	cout << TAILLE[BODYPART_HEAD][1] << endl;
	// taille corps haut
	cout << "taille Chest " << TAILLE[BODYPART_SPINE][1] << " ";
	TAILLE[BODYPART_SPINE][1] = (osList[1]->fin - osList[1]->debut).length()/scale;
	cout << TAILLE[BODYPART_SPINE][1] << endl;
	// taille hanche
	cout << "taille Hips " << TAILLE[BODYPART_PELVIS][1] << " ";
	TAILLE[BODYPART_PELVIS][1] = (osList[1]->debut - osList[0]->debut).length()/scale;
	cout << TAILLE[BODYPART_PELVIS][1] << endl;
	// taille jambe haut gauche
	cout << "taille LeftUpLeg " << TAILLE[BODYPART_LEFT_UPPER_LEG][1] << " ";
	TAILLE[BODYPART_LEFT_UPPER_LEG][1] = (osList[15]->fin - osList[15]->debut).length()/scale;
	cout << TAILLE[BODYPART_LEFT_UPPER_LEG][1] << endl;
	// taille jambe bas gauche
	cout << "taille LeftLowLeg " << TAILLE[BODYPART_LEFT_LOWER_LEG][1] << " ";
	TAILLE[BODYPART_LEFT_LOWER_LEG][1] = (osList[16]->fin - osList[16]->debut).length()/scale;
	cout << TAILLE[BODYPART_LEFT_LOWER_LEG][1] << endl;
	// taille jambe haut droite
	cout << "taille RightUpLeg " << TAILLE[BODYPART_RIGHT_UPPER_LEG][1] << " ";
	TAILLE[BODYPART_RIGHT_UPPER_LEG][1] = (osList[19]->fin - osList[19]->debut).length()/scale;
	cout << TAILLE[BODYPART_RIGHT_UPPER_LEG][1] << endl;
	// taille jambe bas droite
	cout << "taille RightLowLeg " << TAILLE[BODYPART_RIGHT_LOWER_LEG][1] << " ";
	TAILLE[BODYPART_RIGHT_LOWER_LEG][1] = (osList[20]->fin - osList[20]->debut).length()/scale;
	cout << TAILLE[BODYPART_RIGHT_LOWER_LEG][1] << endl;
	// taille bras haut gauche
	cout << "taille LeftUpArm " << TAILLE[BODYPART_LEFT_UPPER_ARM][1] << " ";
	TAILLE[BODYPART_LEFT_UPPER_ARM][1] = (osList[6]->fin - osList[6]->debut).length()/scale;
	cout << TAILLE[BODYPART_LEFT_UPPER_ARM][1] << endl;
	// taille bras bas gauche
	cout << "taille LeftLowArm " << TAILLE[BODYPART_LEFT_LOWER_ARM][1] << " ";
	TAILLE[BODYPART_LEFT_LOWER_ARM][1] = (osList[7]->fin - osList[7]->debut).length()/scale;
	cout << TAILLE[BODYPART_LEFT_LOWER_ARM][1] << endl;
	// taille bras haut droite
	cout << "taille RightUpArm " << TAILLE[BODYPART_RIGHT_UPPER_ARM][1] << " ";
	TAILLE[BODYPART_RIGHT_UPPER_ARM][1] = (osList[11]->fin - osList[11]->debut).length()/scale;
	cout << TAILLE[BODYPART_RIGHT_UPPER_ARM][1] << endl;
	// taille bras bas droite
	cout << "taille RightLowArm " << TAILLE[BODYPART_RIGHT_LOWER_ARM][1] << " ";
	TAILLE[BODYPART_RIGHT_LOWER_ARM][1] = (osList[12]->fin - osList[12]->debut).length()/scale;
	cout << TAILLE[BODYPART_RIGHT_LOWER_ARM][1] << endl;
*/
	// creation des objets graphiques et physiques
	for (int part=0; part<BODYPART_COUNT; part++) {
		POSITIONS[part] = (osList[INDEX_BVH[part]]->debut + osList[INDEX_BVH[part]]->fin)/2.0f;
		TAILLE[part][1] = (osList[INDEX_BVH[part]]->fin - osList[INDEX_BVH[part]]->debut).length()/scale;
		TAILLE[part][0] = TAILLE[part][1]/6.0;
	}

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

	perso = new Perso(bvhFileName, mat, transform);
	stringstream buffer;
	buffer << name << "-sequellette";
	monde3D->add(buffer.str(), perso);
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

void RagDoll::bvhRecursif(int joinId, const Vector3 & accumulateur) {
	// pour tous les fils
	int childId = joint_get_child(frame, joinId);
	int nbrChild = 0;
	Vector3 offset, moyenne = Vector3(0.0f, 0.0f, 0.0f);
	while (childId!=-1) {
		nbrChild++;
		// lire la position de ce joint
		joint_get_offset(frame, childId, offset);

		moyenne += accumulateur+offset;

		// afficher tous les petits fils
		bvhRecursif(childId, accumulateur+offset);

		// passer au fils suivant
		childId = joint_get_next(frame, childId);
	}

	if (nbrChild>=1) {
		osList[joinId] = new Os(accumulateur, moyenne/nbrChild, joinId);
	}
}

btRigidBody * RagDoll::localCreateRigidBody(btScalar mass, const Transform & transform, int bodyPart, const Material & mat) {
	assert(transform.getScale().x == transform.getScale().y == transform.getScale().z);
	f32 scale = transform.getScale().x;
	// position de la partie
	Transform startTransform;
	startTransform.setPosition(transform.getPosition());
	startTransform.setRotation(transform.getOrientation());
	startTransform.translate(POSITIONS[bodyPart]*transform.getScale());
	startTransform.rotate(ROTATIONS[bodyPart]);

	// forme graphique
	meshes[bodyPart] = new Capsule(TAILLE[bodyPart][0]*scale, TAILLE[bodyPart][1]*scale, 8, 4);
	objet3Ds[bodyPart] = new Objet3D(mat, meshes[bodyPart], startTransform);
	stringstream tampon;
	tampon << name << bodyPart;
	monde3D->add(tampon.str(), objet3Ds[bodyPart]);

	// forme physique
	m_shapes[bodyPart] = new btCapsuleShape(TAILLE[bodyPart][0]*scale, TAILLE[bodyPart][1]*scale);
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
