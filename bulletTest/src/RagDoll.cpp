/*
 * RagDoll.cpp
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 *
 *  largement inspire de l'exemple de ragDoll fournit dans bullet
 */

#include "RagDoll.h"


const Quaternion RagDoll::QUATERNION_MOINS_UNIT_Y = Quaternion(M_PI, Vector3::UNIT_Z);
const Quaternion RagDoll::QUATERNION_UNIT_Y = Quaternion();

// TODO importer depuis un fichier
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
	5,	// BODYPART_SPINE corps haut
	1,	// BODYPART_HEAD tete
	5,	// BODYPART_LEFT_UPPER_LEG jambe haut gauche
	5,	// BODYPART_LEFT_LOWER_LEG jambe bas gauche
	5,	// BODYPART_RIGHT_UPPER_LEG jambe haut droite
	5,	// BODYPART_RIGHT_LOWER_LEG jambe bas droite
	4,	// BODYPART_LEFT_UPPER_ARM bras haut gauche
	4,	// BODYPART_LEFT_LOWER_ARM bras bas gauche
	4,	// BODYPART_RIGHT_UPPER_ARM bras haut droite
	4	// BODYPART_RIGHT_LOWER_ARM bras bas droite
};

const f32 RagDoll::POIDS[RagDoll::BODYPART_COUNT] = {
	0.0,	// BODYPART_PELVIS hanche
	0.0,	// BODYPART_SPINE corps haut
	1.0,	// BODYPART_HEAD tete
	1.0,	// BODYPART_LEFT_UPPER_LEG jambe haut gauche
	1.0,	// BODYPART_LEFT_LOWER_LEG jambe bas gauche
	1.0,	// BODYPART_RIGHT_UPPER_LEG jambe haut droite
	1.0,	// BODYPART_RIGHT_LOWER_LEG jambe bas droite
	1.0,	// BODYPART_LEFT_UPPER_ARM bras haut gauche
	1.0,	// BODYPART_LEFT_LOWER_ARM bras bas gauche
	0.5,	// BODYPART_RIGHT_UPPER_ARM bras haut droite
	0.5		// BODYPART_RIGHT_LOWER_ARM bras bas droite
};
// TODO fin importer depuis un fichier

// constante qui definissent l'ensemble des contraintes d'une ragDoll
const int RagDoll::CONTRAINTES_BODY[RagDoll::JOINT_COUNT][2] = {
	{BODYPART_PELVIS, BODYPART_SPINE},					// JOINT_PELVIS_SPINE
	{BODYPART_SPINE, BODYPART_HEAD},					// JOINT_SPINE_HEAD
	{BODYPART_PELVIS, BODYPART_LEFT_UPPER_LEG},			// JOINT_LEFT_HIP
	{BODYPART_LEFT_UPPER_LEG, BODYPART_LEFT_LOWER_LEG},	// JOINT_LEFT_KNEE
	{BODYPART_PELVIS, BODYPART_RIGHT_UPPER_LEG},		// JOINT_RIGHT_HIP
	{BODYPART_RIGHT_UPPER_LEG, BODYPART_RIGHT_LOWER_LEG},	// JOINT_RIGHT_KNEE
	{BODYPART_SPINE, BODYPART_LEFT_UPPER_ARM},				// JOINT_LEFT_SHOULDER
	{BODYPART_LEFT_UPPER_ARM, BODYPART_LEFT_LOWER_ARM},		// JOINT_LEFT_ELBOW
	{BODYPART_SPINE, BODYPART_RIGHT_UPPER_ARM},				// JOINT_RIGHT_SHOULDER
	{BODYPART_RIGHT_UPPER_ARM, BODYPART_RIGHT_LOWER_ARM}	// JOINT_RIGHT_ELBOW
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

const btVector3 RagDoll::CONTRAINTES_LIMITES_ANGLES[RagDoll::JOINT_COUNT][2] = {
	{btVector3(-M_PI_4, 0.0, 0.0),	btVector3(M_PI_2, 0.0, 0.0)},	//{-M_PI_4, M_PI_2, 0.0f},		// JOINT_PELVIS_SPINE
	{btVector3(-M_PI_4, -M_PI_4, -M_PI_4),		btVector3(M_PI_4, M_PI_4, M_PI_4)},		//{M_PI_4, M_PI_4, M_PI_2},		// JOINT_SPINE_HEAD
	{btVector3(-M_PI_4, -M_PI_4, -M_PI_4),		btVector3(M_PI_4, M_PI_4, M_PI_4)},		//{M_PI_4, M_PI_4, 0},			// JOINT_LEFT_HIP
	{btVector3(0.0f, 0.0, 0.0),		btVector3(M_PI_2, 0.0, 0.0)},	//{0.0f, M_PI_2, 0.0f},			// JOINT_LEFT_KNEE
	{btVector3(-M_PI_4, -M_PI_4, -M_PI_4),		btVector3(M_PI_4, M_PI_4, M_PI_4)},		//{M_PI_4, M_PI_4, 0.0f},		// JOINT_RIGHT_HIP
	{btVector3(0.0f, 0.0, 0.0),		btVector3(M_PI_2, 0.0, 0.0)},	//{0.0f, M_PI_2, 0.0f},			// JOINT_RIGHT_KNEE
	{btVector3(-M_PI_2, -M_PI_2, -M_PI_2),		btVector3(M_PI_2, M_PI_2, M_PI_2)},		//{M_PI_2, M_PI_2, 0.0f},		// JOINT_LEFT_SHOULDER
	{btVector3(-M_PI_2, 0.0, 0.0),	btVector3(0.0f, 0.0, 0.0)},		//{-M_PI_2, 0.0f, 0.0f},		// JOINT_LEFT_ELBOW
	{btVector3(-M_PI_2, -M_PI_2, -M_PI_2),		btVector3(M_PI_2, M_PI_2, M_PI_2)},		//{M_PI_2, M_PI_2, 0},			// JOINT_RIGHT_SHOULDER
	{btVector3(-M_PI_2, 0.0, 0.0),	btVector3(0.0f, 0.0, 0.0)}		//{-M_PI_2, 0.0f, 0.0f}			// JOINT_RIGHT_ELBOW
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

Quaternion RagDoll::getOrientationOs(const Vector3 & v) {
	Quaternion rotations;
	if (v.x!=0.0f or v.z!=0.0f)
		return Vector3::UNIT_Y.getRotationTo(v);
	// else {
	if (v.y<0.0f)
		return QUATERNION_MOINS_UNIT_Y;
	//else
	return QUATERNION_UNIT_Y;
	//}
}

RagDoll::RagDoll (const string & name, const string & bvhFileName, const Material & mat, const Transform & transform, const Quaternion & orientationEdition, btDiscreteDynamicsWorld * m_ownerWorld, Monde3D * monde3D)
: name(name), bvhFileName(bvhFileName), orientationEdition(orientationEdition), m_ownerWorld(m_ownerWorld), monde3D(monde3D), temps(0.0), sens(true) {
	assert(transform.getScale().x == transform.getScale().y == transform.getScale().z);
	scale = transform.getScale().x;

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

	// lecture de la position des extremite des os du bvh dans la position d'edition dans le repere de l'animation
	vector<Os *> osList;
	osList.resize(motion_frame_get_joints_n(frame), NULL);
	bvhRecursif(osList, rootId, offset);
	vector<Os *>::iterator it = osList.begin();

	// creation des objets graphiques et physiques
	Transform localTransformPart; f32 hauteur; f32 rayon;
	Transform globalTransformRagDoll(transform.getPosition(), transform.getOrientation());
	for (int part=0; part<BODYPART_COUNT; part++) {
		// calcul de la longueur et du rayon de l'os (distance entre les 2 extreminte de l'os)
		Vector3 taille = osList[indexBvh[part]]->fin - osList[indexBvh[part]]->debut;
		hauteur = taille.length();
		rayon = hauteur/RAPPORT_HAUTEURS_RAYONS[part];
		// calcul de la position du centre de l'os dans le repere de l'animation
		localTransformPart.setPosition((osList[indexBvh[part]]->debut + osList[indexBvh[part]]->fin)/2.0f);
		// calcul de l'orientation de l'os
		localTransformPart.setRotation(getOrientationOs(taille));
		// creation de la forme physique et graphique
		Transform globalTransformPart = globalTransformRagDoll * localTransformPart;
		m_bodies[part] = localCreateRigidBody(POIDS[part], hauteur, rayon, globalTransformPart, part, mat);
	}

	// creation des jointures
	btTransform pereTrans, filsTrans;
	for (int joint=0; joint<JOINT_COUNT; joint++) {
		// calcul de la position du point de jointure dans le repere de l'os fils
		filsTrans = btTransform(CONTRAINTES_ORIENTATIONS[joint][1], btVector3(0.0f, -m_shapes[CONTRAINTES_BODY[joint][1]]->getHalfHeight(), 0.0f));
		// calculer la position de l'os pere dans le repere de l'os pere par rapport a la position de l'os fils dans le repere de l'os fils
		pereTrans = btTransform(CONTRAINTES_ORIENTATIONS[joint][0], (m_bodies[CONTRAINTES_BODY[joint][0]]->getCenterOfMassTransform().inverse()(m_bodies[CONTRAINTES_BODY[joint][1]]->getCenterOfMassTransform()(filsTrans.getOrigin()))));
		if (CONTRAINTES_IS_CONE[joint]) {
			// contrainte de type epaule, hanche : 3 degres de liberte
			m_joints[joint] = new btGeneric6DofConstraint(*m_bodies[CONTRAINTES_BODY[joint][0]], *m_bodies[CONTRAINTES_BODY[joint][1]], pereTrans, filsTrans, true);
			static_cast<btGeneric6DofConstraint*>(m_joints[joint])->setAngularLowerLimit(CONTRAINTES_LIMITES_ANGLES[joint][0]);
			static_cast<btGeneric6DofConstraint*>(m_joints[joint])->setAngularUpperLimit(CONTRAINTES_LIMITES_ANGLES[joint][1]);
//			m_joints[joint] = new btConeTwistConstraint(*m_bodies[CONTRAINTES_BODY[joint][0]], *m_bodies[CONTRAINTES_BODY[joint][1]], pereTrans, filsTrans);
//			((btConeTwistConstraint*)m_joints[joint])->setLimit(CONTRAINTES_LIMITES_ANGLES[joint][0], CONTRAINTES_LIMITES_ANGLES[joint][1], CONTRAINTES_LIMITES_ANGLES[joint][2]);
			m_ownerWorld->addConstraint(m_joints[joint], true);
		} else {
			// constrainte de type genoux, coude :  1 seul degres de liberte
			m_joints[joint] = new btHingeConstraint(*m_bodies[CONTRAINTES_BODY[joint][0]], *m_bodies[CONTRAINTES_BODY[joint][1]], pereTrans, filsTrans);
			static_cast<btHingeConstraint*>(m_joints[joint])->setLimit(CONTRAINTES_LIMITES_ANGLES[joint][0][0], CONTRAINTES_LIMITES_ANGLES[joint][1][0]);
			m_ownerWorld->addConstraint(m_joints[joint], true);
		}
	}

	// affichage de la bvh
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

void RagDoll::bvhRecursif(vector<Os *> & osList, int joinId, const Vector3 & accumulateur) {
	// pour tous les fils
	int childId = joint_get_child(frame, joinId);
	int nbrChild = 0;
	Vector3 offset, moyenne = Vector3(0.0f, 0.0f, 0.0f);
	while (childId!=-1) {
		nbrChild++;
		// lire la position de ce joint
		joint_get_offset(frame, childId, offset);
		offset = orientationEdition * offset * scale;

		moyenne += accumulateur+offset;

		// afficher tous les petits fils
		bvhRecursif(osList, childId, accumulateur+offset);

		// passer au fils suivant
		childId = joint_get_next(frame, childId);
	}

	if (nbrChild>=1) {
		osList[joinId] = new Os(accumulateur, moyenne/nbrChild, joinId);
	}
}

btRigidBody * RagDoll::localCreateRigidBody(btScalar mass, f32 hauteur, f32 rayon, const Transform & globalTransformPart, int bodyPart, const Material & mat) {
	// forme graphique
	meshes[bodyPart] = new Capsule(rayon, hauteur, 8, 4);
	objet3Ds[bodyPart] = new Objet3D(mat, meshes[bodyPart], globalTransformPart);
	stringstream tampon;
	tampon << name << bodyPart;
	monde3D->add(tampon.str(), objet3Ds[bodyPart]);

	// forme physique
	m_shapes[bodyPart] = new btCapsuleShape(rayon, hauteur);
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
	static f32 vitesse = 1.0;
	static f32 maxMotorForce = 300.0;
	static f32 maxImpulseHinge = 20.0;
//	static f32 maxImpulseTwist = 200.0;

	btGeneric6DofConstraint * jointLeftHip = static_cast<btGeneric6DofConstraint*>(m_joints[JOINT_LEFT_HIP]);
	btGeneric6DofConstraint * jointRightHip = static_cast<btGeneric6DofConstraint*>(m_joints[JOINT_RIGHT_HIP]);
//	btGeneric6DofConstraint * jointLeftShoulder = static_cast<btGeneric6DofConstraint*>(m_joints[JOINT_LEFT_SHOULDER]);
//	btGeneric6DofConstraint * jointRightShoulder = static_cast<btGeneric6DofConstraint*>(m_joints[JOINT_RIGHT_SHOULDER]);

	btHingeConstraint * jointLeftKnee = static_cast<btHingeConstraint*>(m_joints[JOINT_LEFT_KNEE]);
	btHingeConstraint * jointRightKnee = static_cast<btHingeConstraint*>(m_joints[JOINT_RIGHT_KNEE]);
	btHingeConstraint * jointLeftElbow = static_cast<btHingeConstraint*>(m_joints[JOINT_LEFT_ELBOW]);
	btHingeConstraint * jointRightElbow = static_cast<btHingeConstraint*>(m_joints[JOINT_RIGHT_ELBOW]);
	btHingeConstraint * jointPelvisSpine = static_cast<btHingeConstraint*>(m_joints[JOINT_PELVIS_SPINE]);

	temps += elapsed;
	if (temps>1.0) {
		temps = 0.0;
		sens = !sens;

		if (sens) {
			cout << "haut" << endl;
			for (int axe=0; axe<3; axe++) {
				jointLeftHip->getRotationalLimitMotor(axe)->m_enableMotor = true;
				jointLeftHip->getRotationalLimitMotor(axe)->m_targetVelocity = vitesse;
				jointLeftHip->getRotationalLimitMotor(axe)->m_maxMotorForce = maxMotorForce;
			}
			for (int axe=0; axe<3; axe++) {
				jointRightHip->getRotationalLimitMotor(axe)->m_enableMotor = true;
				jointRightHip->getRotationalLimitMotor(axe)->m_targetVelocity = vitesse;
				jointRightHip->getRotationalLimitMotor(axe)->m_maxMotorForce = maxMotorForce;
			}

			jointLeftKnee->enableAngularMotor(true, vitesse, maxImpulseHinge);
			jointRightKnee->enableAngularMotor(true, vitesse, maxImpulseHinge);
			jointLeftElbow->enableAngularMotor(true, -vitesse, maxImpulseHinge);
			jointRightElbow->enableAngularMotor(true, -vitesse, maxImpulseHinge);
			jointPelvisSpine->enableAngularMotor(true, vitesse, maxImpulseHinge);
		} else {
			cout << "bas" << endl;
			for (int axe=0; axe<3; axe++) {
				jointLeftHip->getRotationalLimitMotor(axe)->m_enableMotor = true;
				jointLeftHip->getRotationalLimitMotor(axe)->m_targetVelocity = -vitesse;
				jointLeftHip->getRotationalLimitMotor(axe)->m_maxMotorForce = maxMotorForce;
			}
			for (int axe=0; axe<3; axe++) {
				jointRightHip->getRotationalLimitMotor(axe)->m_enableMotor = true;
				jointRightHip->getRotationalLimitMotor(axe)->m_targetVelocity = -vitesse;
				jointRightHip->getRotationalLimitMotor(axe)->m_maxMotorForce = maxMotorForce;
			}
			jointLeftKnee->enableAngularMotor(true, -vitesse, maxImpulseHinge);
			jointRightKnee->enableAngularMotor(true, -vitesse, maxImpulseHinge);
			jointLeftElbow->enableAngularMotor(true, vitesse, maxImpulseHinge);
			jointRightElbow->enableAngularMotor(true, vitesse, maxImpulseHinge);
			jointPelvisSpine->enableAngularMotor(true, -vitesse, maxImpulseHinge);
		}
	}
}
