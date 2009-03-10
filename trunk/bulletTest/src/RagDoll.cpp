/*
 * RagDoll.cpp
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 *
 *  largement inspire de l'exemple de ragDoll fournit dans bullet
 */

#include "RagDoll.h"



const int RagDoll::INDEX_EXAMPLE1[RagDoll::BODYPART_COUNT] = {
	0,		// hanche
	1,		// corps haut
	3,		// tete
	15,		// jambe haut gauche
	16,		// jambe bas gauche
	19,		// jambe haut droite
	20,		// jambe bas droite
	6, 		// bras haut gauche
	7, 		// bras bas gauche
	11,		// bras haut droite
	12		// bras bas droite
};

const int RagDoll::INDEX_WALK[RagDoll::BODYPART_COUNT] = {
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

const int RagDoll::RAPPORT_HAUTEURS_RAYONS[RagDoll::BODYPART_COUNT] = {
	2,	// BODYPART_PELVIS hanche
	4,	// BODYPART_SPINE corps haut
	1,	// BODYPART_HEAD tete
	4,	// BODYPART_LEFT_UPPER_LEG jambe haut gauche
	4,	// BODYPART_LEFT_LOWER_LEG jambe bas gauche
	4,	// BODYPART_RIGHT_UPPER_LEG jambe haut droite
	4,	// BODYPART_RIGHT_LOWER_LEG jambe bas droite
	4,	// BODYPART_LEFT_UPPER_ARM bras haut gauche
	4,	// BODYPART_LEFT_LOWER_ARM bras bas gauche
	4,	// BODYPART_RIGHT_UPPER_ARM bras haut droite
	4	// BODYPART_RIGHT_LOWER_ARM bras bas droite
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

const btQuaternion RagDoll::CONTRAINTES_ORIENTATIONS[RagDoll::JOINT_COUNT][2] = {
	{btQuaternion(M_PI_2,0,0), btQuaternion(M_PI_2,0,0)},			// JOINT_PELVIS_SPINE
	{btQuaternion(0,0,M_PI_2), btQuaternion(0,0,M_PI_2)},			// JOINT_SPINE_HEAD
	{btQuaternion(0,0,-M_PI_4*1), btQuaternion(0,0,-M_PI_4*5)},		// JOINT_LEFT_HIP
	{btQuaternion(M_PI_2,0,0), btQuaternion(M_PI_2,0,0)},			// JOINT_LEFT_KNEE
	{btQuaternion(0,0,M_PI_4*5), btQuaternion(0,0,M_PI_4)},			// JOINT_RIGHT_HIP
	{btQuaternion(M_PI_2,0,0), btQuaternion(M_PI_2,0,0)},			// JOINT_RIGHT_KNEE
	{btQuaternion(0,0,0), btQuaternion(0,0,M_PI_2)},				// JOINT_LEFT_SHOULDER
	{btQuaternion(M_PI_2,0,0), btQuaternion(M_PI_2,0,0)},			// JOINT_LEFT_ELBOW
	{btQuaternion(0,0,M_PI), btQuaternion(0,0,M_PI_2)},				// JOINT_RIGHT_SHOULDER
	{btQuaternion(M_PI_2,0,0), btQuaternion(M_PI_2,0,0)}			// JOINT_RIGHT_ELBOW
};

const f32 RagDoll::CONTRAINTES_LIMITES_ANGLES[RagDoll::JOINT_COUNT][3] = {
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

f32 RagDoll::tailles[RagDoll::BODYPART_COUNT][2] =
{
		{0.15, 0.20}, // BODYPART_PELVIS
		{0.15, 0.28}, // BODYPART_SPINE
		{0.10, 0.05}, // BODYPART_HEAD
		{0.07, 0.45}, // BODYPART_LEFT_UPPER_LEG
		{0.05, 0.37}, // BODYPART_LEFT_LOWER_LEG
		{0.07, 0.45}, // BODYPART_RIGHT_UPPER_LEG
		{0.05, 0.37}, // BODYPART_RIGHT_LOWER_LEG
		{0.05, 0.33}, // BODYPART_LEFT_UPPER_ARM
		{0.04, 0.25}, // BODYPART_LEFT_LOWER_ARM
		{0.05, 0.33}, // BODYPART_RIGHT_UPPER_ARM
		{0.04, 0.25}  // BODYPART_RIGHT_LOWER_ARM
};

Vector3 RagDoll::positions[RagDoll::BODYPART_COUNT] = {
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

Quaternion RagDoll::rotations[RagDoll::BODYPART_COUNT] =
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

btTransform RagDoll::contraintesPositions[RagDoll::JOINT_COUNT][2] = {
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

RagDoll::RagDoll (const string & name, const string & bvhFileName, const Material & mat, const Transform & transform, const Quaternion & orientationEdition, btDiscreteDynamicsWorld * m_ownerWorld, Monde3D * monde3D)
: name(name), bvhFileName(bvhFileName), orientationEdition(orientationEdition), m_ownerWorld(m_ownerWorld), monde3D(monde3D) {
	const int * indexBvh;
	if (bvhFileName == "data/Example1.bvh")
		indexBvh = INDEX_EXAMPLE1;
	else if (bvhFileName == "data/walk.bvh")
		indexBvh = INDEX_WALK;
	else
		throw Erreur("bvh inconu");

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

	// lecture de la position des joints dans le bvh dans la position d'edition
	osList.resize(motion_frame_get_joints_n(frame), NULL);
	bvhRecursif(rootId, offset);
	vector<Os *>::iterator it = osList.begin();
	while (it!=osList.end()) {
		if (*it!=NULL)
			cout << (*it)->joint << " " << joint_get_name(frame, (*it)->joint) << " " << ((*it)->fin - (*it)->debut) << endl;
		it++;
	}

	Quaternion q;
	q.FromAngleAxis(M_PI, Vector3::UNIT_Z);
	// creation des objets graphiques et physiques
	for (int part=0; part<BODYPART_COUNT; part++) {
		Vector3 taille = osList[indexBvh[part]]->fin - osList[indexBvh[part]]->debut;
		tailles[part][1] = taille.length();
		tailles[part][0] = tailles[part][1]/RAPPORT_HAUTEURS_RAYONS[part];
		positions[part] = (osList[indexBvh[part]]->debut + osList[indexBvh[part]]->fin)/2.0f;
		if (taille.x!=0.0f or taille.z!=0.0f)
			rotations[part] = Vector3::UNIT_Y.getRotationTo(taille);
		else {
			if (taille.y<0.0f)
				rotations[part] = q;
			else
				rotations[part] = Quaternion();
			cout << taille << endl;
		}
	}

	// creation des objets graphiques et physiques
	for (int part=0; part<BODYPART_COUNT; part++)
		m_bodies[part] = localCreateRigidBody(1.0f, transform, part, mat);

	// creation des jointures
	f32 scale = transform.getScale().x;

	// Now setup the constraints
	for (int joint=0; joint<JOINT_COUNT; joint++) {
		contraintesPositions[joint][0] = btTransform(CONTRAINTES_ORIENTATIONS[joint][0], btVector3(0.0f, tailles[CONTRAINTES_BODY[joint][0]][1], 0.0f)*scale/2.0f);
		contraintesPositions[joint][1] = btTransform(CONTRAINTES_ORIENTATIONS[joint][1], btVector3(0.0f, -tailles[CONTRAINTES_BODY[joint][1]][1], 0.0f)*scale/2.0f);
		if (CONTRAINTES_IS_CONE[joint]) {
//			if (joint==JOINT_LEFT_HIP) {
				contraintesPositions[joint][1] = btTransform(CONTRAINTES_ORIENTATIONS[joint][1], btVector3(0.0f, -tailles[CONTRAINTES_BODY[joint][1]][1], 0.0f)*scale/2.0f);
				// calculer la position de la 2eme par rapport a la premiere
				contraintesPositions[joint][0] = btTransform(CONTRAINTES_ORIENTATIONS[joint][0], (m_bodies[CONTRAINTES_BODY[joint][0]]->getCenterOfMassTransform().inverse()(m_bodies[CONTRAINTES_BODY[joint][1]]->getCenterOfMassTransform()(contraintesPositions[joint][1].getOrigin()))));
			m_joints[joint] = new btConeTwistConstraint(*m_bodies[CONTRAINTES_BODY[joint][0]], *m_bodies[CONTRAINTES_BODY[joint][1]], contraintesPositions[joint][0], contraintesPositions[joint][1]);
			((btConeTwistConstraint*)m_joints[joint])->setLimit(CONTRAINTES_LIMITES_ANGLES[joint][0], CONTRAINTES_LIMITES_ANGLES[joint][1], CONTRAINTES_LIMITES_ANGLES[joint][2]);
			m_ownerWorld->addConstraint(m_joints[joint], true);
//			}
		} else {
			m_joints[joint] = new btHingeConstraint(*m_bodies[CONTRAINTES_BODY[joint][0]], *m_bodies[CONTRAINTES_BODY[joint][1]], contraintesPositions[joint][0], contraintesPositions[joint][1]);
			((btHingeConstraint*)m_joints[joint])->setLimit(CONTRAINTES_LIMITES_ANGLES[joint][0], CONTRAINTES_LIMITES_ANGLES[joint][1]);
			m_ownerWorld->addConstraint(m_joints[joint], true);
		}
	}

	Transform t = transform;
	t.rotate(orientationEdition);
	perso = new Perso(bvhFileName, mat, t);
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
		offset = orientationEdition * offset;

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
	// position de la partie dans le repere du ragDoll
	Transform localTransformPart;
	localTransformPart.setPosition(positions[bodyPart]*transform.getScale());
	localTransformPart.setRotation(rotations[bodyPart]);
	// position du ragDoll dans le repere du monde
	Transform globalTransformRagDoll;
	globalTransformRagDoll.setPosition(transform.getPosition());
	globalTransformRagDoll.setRotation(transform.getOrientation());
	// position de la partie dans le repere du monde
	Transform globalTransformPart = globalTransformRagDoll * localTransformPart;

	// forme graphique
	meshes[bodyPart] = new Capsule(tailles[bodyPart][0]*scale, tailles[bodyPart][1]*scale, 8, 4);
	objet3Ds[bodyPart] = new Objet3D(mat, meshes[bodyPart], globalTransformPart);
	stringstream tampon;
	tampon << name << bodyPart;
	monde3D->add(tampon.str(), objet3Ds[bodyPart]);

	// forme physique
	m_shapes[bodyPart] = new btCapsuleShape(tailles[bodyPart][0]*scale, tailles[bodyPart][1]*scale);
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
