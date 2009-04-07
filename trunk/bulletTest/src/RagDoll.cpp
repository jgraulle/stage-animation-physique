/*
 * RagDoll.cpp
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 *
 *  largement inspire de l'exemple de ragDoll fournit dans bullet
 */

#include "RagDoll.h"

#include <fstream>

const Quaternion RagDoll::QUATERNION_MOINS_UNIT_Y = Quaternion(M_PI, Vector3::UNIT_Z);
const Quaternion RagDoll::QUATERNION_UNIT_Y = Quaternion();

// constante qui definissent l'ensemble des contraintes d'une ragDoll
const int RagDoll::CONTRAINTES_BODY[RagDoll::JOINT_COUNT][2] = { { BODYPART_PELVIS, BODYPART_SPINE }, // JOINT_PELVIS_SPINE
		{ BODYPART_SPINE, BODYPART_HEAD }, // JOINT_SPINE_HEAD
		{ BODYPART_PELVIS, BODYPART_LEFT_UPPER_LEG }, // JOINT_LEFT_HIP
		{ BODYPART_LEFT_UPPER_LEG, BODYPART_LEFT_LOWER_LEG }, // JOINT_LEFT_KNEE
		{ BODYPART_PELVIS, BODYPART_RIGHT_UPPER_LEG }, // JOINT_RIGHT_HIP
		{ BODYPART_RIGHT_UPPER_LEG, BODYPART_RIGHT_LOWER_LEG }, // JOINT_RIGHT_KNEE
		{ BODYPART_SPINE, BODYPART_LEFT_UPPER_ARM }, // JOINT_LEFT_SHOULDER
		{ BODYPART_LEFT_UPPER_ARM, BODYPART_LEFT_LOWER_ARM }, // JOINT_LEFT_ELBOW
		{ BODYPART_SPINE, BODYPART_RIGHT_UPPER_ARM }, // JOINT_RIGHT_SHOULDER
		{ BODYPART_RIGHT_UPPER_ARM, BODYPART_RIGHT_LOWER_ARM } // JOINT_RIGHT_ELBOW
};

const btQuaternion RagDoll::CONTRAINTES_ORIENTATIONS[RagDoll::JOINT_COUNT][2] = { { btQuaternion(M_PI_2,0, 0),
		btQuaternion(M_PI_2,0, 0) }, // JOINT_PELVIS_SPINE
		{ btQuaternion(0, 0, M_PI_2), btQuaternion(0, 0, M_PI_2) }, // JOINT_SPINE_HEAD
		{ btQuaternion(0, 0, -M_PI_4 * 1), btQuaternion(0, 0, -M_PI_4 * 5) }, // JOINT_LEFT_HIP
		{ btQuaternion(M_PI_2,0, 0), btQuaternion(M_PI_2,0, 0) }, // JOINT_LEFT_KNEE
		{ btQuaternion(0, 0, M_PI_4*5), btQuaternion(0, 0, M_PI_4) }, // JOINT_RIGHT_HIP
		{ btQuaternion(M_PI_2,0, 0), btQuaternion(M_PI_2,0, 0) }, // JOINT_RIGHT_KNEE
		{ btQuaternion(0, 0, 0), btQuaternion(0, 0, M_PI_2) }, // JOINT_LEFT_SHOULDER
		{ btQuaternion(M_PI_2,0, 0), btQuaternion(M_PI_2,0, 0) }, // JOINT_LEFT_ELBOW
		{ btQuaternion(0, 0, M_PI), btQuaternion(0, 0, M_PI_2) }, // JOINT_RIGHT_SHOULDER
		{ btQuaternion(M_PI_2,0, 0), btQuaternion(M_PI_2,0, 0) } // JOINT_RIGHT_ELBOW
};

const btVector3 RagDoll::CONTRAINTES_LIMITES_ANGLES[RagDoll::JOINT_COUNT][2] = { { btVector3(-M_PI_4, 0.0, 0.0),
		btVector3(M_PI_2, 0.0, 0.0) }, //{-M_PI_4, M_PI_2, 0.0f},		// JOINT_PELVIS_SPINE
		{ btVector3(-M_PI_4, -M_PI_4, -M_PI_4), btVector3(M_PI_4, M_PI_4, M_PI_4) }, //{M_PI_4, M_PI_4, M_PI_2},		// JOINT_SPINE_HEAD
		{ btVector3(-M_PI_4, -M_PI_4, -M_PI_4), btVector3(M_PI_4, M_PI_4, M_PI_4) }, //{M_PI_4, M_PI_4, 0},			// JOINT_LEFT_HIP
		{ btVector3(0.0f, 0.0, 0.0), btVector3(M_PI_2, 0.0, 0.0) }, //{0.0f, M_PI_2, 0.0f},			// JOINT_LEFT_KNEE
		{ btVector3(-M_PI_4, -M_PI_4, -M_PI_4), btVector3(M_PI_4, M_PI_4, M_PI_4) }, //{M_PI_4, M_PI_4, 0.0f},		// JOINT_RIGHT_HIP
		{ btVector3(0.0f, 0.0, 0.0), btVector3(M_PI_2, 0.0, 0.0) }, //{0.0f, M_PI_2, 0.0f},			// JOINT_RIGHT_KNEE
		{ btVector3(-M_PI_2, -M_PI_2, -M_PI_2), btVector3(M_PI_2, M_PI_2, M_PI_2) }, //{M_PI_2, M_PI_2, 0.0f},		// JOINT_LEFT_SHOULDER
		{ btVector3(-M_PI_2, 0.0, 0.0), btVector3(0.0f, 0.0, 0.0) }, //{-M_PI_2, 0.0f, 0.0f},		// JOINT_LEFT_ELBOW
		{ btVector3(-M_PI_2, -M_PI_2, -M_PI_2), btVector3(M_PI_2, M_PI_2, M_PI_2) }, //{M_PI_2, M_PI_2, 0},			// JOINT_RIGHT_SHOULDER
		{ btVector3(-M_PI_2, 0.0, 0.0), btVector3(0.0f, 0.0, 0.0) } //{-M_PI_2, 0.0f, 0.0f}			// JOINT_RIGHT_ELBOW
};

const bool RagDoll::CONTRAINTES_IS_CONE[RagDoll::JOINT_COUNT] = { false, true, true, false, true, false, true, false,
		true, false };

Quaternion RagDoll::getOrientationOs(const Vector3 & v) {
	Quaternion rotations;
	if (v.x != 0.0f or v.z != 0.0f)
		return Vector3::UNIT_Y.getRotationTo(v);
	// else {
	if (v.y < 0.0f)
		return QUATERNION_MOINS_UNIT_Y;
	//else
	return QUATERNION_UNIT_Y;
	//}
}

RagDoll::RagDoll(const string & name, const SkeletonMesh * bvhFileName, const Material & mat,
		const Transform & transform, btDiscreteDynamicsWorld * m_ownerWorld, Monde3D * monde3D) :
			Objet3D(mat, NULL, transform), name(name), skeletonMesh(bvhFileName), m_ownerWorld(m_ownerWorld), monde3D(monde3D), temps(
			0.0), sens(true) {
	assert((transform.getScale().x == transform.getScale().y) && (transform.getScale().y== transform.getScale().z));
	scale = transform.getScale().x;

	// ouverture du fichier de description du bvh
	string txtFileName = skeletonMesh->getBvhFileName().substr(0, skeletonMesh->getBvhFileName().size() - 3) + "txt";
	ifstream fichier(txtFileName.c_str());
	if (fichier.fail())
		throw ErreurFileNotFound(txtFileName.c_str(), "TODO"); // TODO donner le texte associe a l'erreur
	string line;

	// lecture du nom du fichier bvh
	if (!(fichier >> line))
		throw Erreur("erreur de lecture dans le fichier '" + txtFileName + "' !");

	// lecture du nombre de partie du corps
	int nbrBodyPart;
	if (!(fichier >> nbrBodyPart))
		throw Erreur("erreur de lecture dans le fichier '" + txtFileName + "' !");
	if (nbrBodyPart != BODYPART_COUNT) {
		ostringstream o;
		o << "le fichier '" << txtFileName << "' ne contient que " << nbrBodyPart << " partie du corps à la place de "
				<< BODYPART_COUNT << " !";
		throw Erreur(o.str());
	}
	// lire les informations sur chaque partie
	for (int i = 0; i < BODYPART_COUNT; i++) {
		// lire le nom de la partie physique
		if (!(fichier >> line)) {
			ostringstream o;
			o << "erreur de lecture du nom de la part " << i << " dans le fichier '" << txtFileName << "' !";
			throw Erreur(o.str());
		}

		// lire le nom du joint bvh
		if (!(fichier >> line)) {
			ostringstream o;
			o << "erreur de lecture du nom de la part " << i << " dans le fichier '" << txtFileName << "' !";
			throw Erreur(o.str());
		}
		// convertion du nom en index
		bodyIndex[i] = skeletonMesh->getJointIndex(line);

		// lire le rayon
		if (!(fichier >> rapportHauteursRayons[i])) {
			ostringstream o;
			o << "erreur de lecture du rayon de la part " << i << " dans le fichier '" << txtFileName << "' !";
			throw Erreur(o.str());
		}
		// lire le poids
		if (!(fichier >> poids[i])) {
			ostringstream o;
			o << "erreur de lecture du poids de la part " << i << " dans le fichier '" << txtFileName << "' !";
			throw Erreur(o.str());
		}
	}

	// creation des objets graphiques et physiques
	Transform localTransformPart;
	f32 hauteur;
	f32 rayon;
	Transform globalTransformRagDoll(transform.getPosition(), transform.getOrientation());
	for (int part = 0; part < BODYPART_COUNT; part++) {
		// calcul de la longueur et du rayon de l'os (distance entre les 2 extreminte de l'os)
		Vector3 taille = skeletonMesh->getOsPosEdition(bodyIndex[part])->fin - skeletonMesh->getOsPosEdition(
				bodyIndex[part])->debut;
		hauteur = taille.length();
		rayon = hauteur / rapportHauteursRayons[part];
		// calcul de la position du centre de l'os dans le repere de l'animation
		localTransformPart.setPosition((skeletonMesh->getOsPosEdition(bodyIndex[part])->debut
				+ skeletonMesh->getOsPosEdition(bodyIndex[part])->fin) / 2.0f);
		// calcul de l'orientation de l'os
		localTransformPart.setRotation(getOrientationOs(taille));
		// creation de la forme physique et graphique
		Transform globalTransformPart = globalTransformRagDoll * localTransformPart;
		m_bodies[part] = localCreateRigidBody(poids[part], hauteur, rayon, globalTransformPart, part, mat);
	}

	// creation des jointures
	btTransform pereTrans, filsTrans;
	for (int joint = 0; joint < JOINT_COUNT; joint++) {
		// calcul de la position du point de jointure dans le repere de l'os fils
		filsTrans = btTransform(CONTRAINTES_ORIENTATIONS[joint][1], btVector3(0.0f,
				-m_shapes[CONTRAINTES_BODY[joint][1]]->getHalfHeight(), 0.0f));
		// calculer la position de l'os pere dans le repere de l'os pere par rapport a la position de l'os fils dans le repere de l'os fils
		pereTrans = btTransform(CONTRAINTES_ORIENTATIONS[joint][0],
				(m_bodies[CONTRAINTES_BODY[joint][0]]->getCenterOfMassTransform().inverse()(
						m_bodies[CONTRAINTES_BODY[joint][1]]->getCenterOfMassTransform()(filsTrans.getOrigin()))));
		if (CONTRAINTES_IS_CONE[joint]) {
			// contrainte de type epaule, hanche : 3 degres de liberte
			m_jointsGeneric6[joint] = new btGeneric6DofConstraint(*m_bodies[CONTRAINTES_BODY[joint][0]],
					*m_bodies[CONTRAINTES_BODY[joint][1]], pereTrans, filsTrans, true);
			m_jointsHinge[joint] = NULL;
			m_joints[joint] = m_jointsGeneric6[joint];
			m_jointsGeneric6[joint]->setAngularLowerLimit(CONTRAINTES_LIMITES_ANGLES[joint][0]);
			m_jointsGeneric6[joint]->setAngularUpperLimit(CONTRAINTES_LIMITES_ANGLES[joint][1]);
			//			m_joints[joint] = new btConeTwistConstraint(*m_bodies[CONTRAINTES_BODY[joint][0]], *m_bodies[CONTRAINTES_BODY[joint][1]], pereTrans, filsTrans);
			//			((btConeTwistConstraint*)m_joints[joint])->setLimit(CONTRAINTES_LIMITES_ANGLES[joint][0], CONTRAINTES_LIMITES_ANGLES[joint][1], CONTRAINTES_LIMITES_ANGLES[joint][2]);
			m_ownerWorld->addConstraint(m_joints[joint], true);
		} else {
			// constrainte de type genoux, coude :  1 seul degres de liberte
			m_jointsHinge[joint] = new btHingeConstraint(*m_bodies[CONTRAINTES_BODY[joint][0]],
					*m_bodies[CONTRAINTES_BODY[joint][1]], pereTrans, filsTrans);
			m_jointsGeneric6[joint] = NULL;
			m_joints[joint] = m_jointsHinge[joint];
			m_jointsHinge[joint]->setLimit(CONTRAINTES_LIMITES_ANGLES[joint][0][0], CONTRAINTES_LIMITES_ANGLES[joint][1][0]);
			m_ownerWorld->addConstraint(m_joints[joint], true);
		}
	}

	// creation du squelette de debug
	skeleton = new Skeleton(skeletonMesh, mat, transform);
}

RagDoll::~RagDoll() {
	int i;

	// Remove all constraints
	for (i = 0; i < JOINT_COUNT; ++i) {
		m_ownerWorld->removeConstraint(m_joints[i]);
		delete m_joints[i];
		m_joints[i] = 0;
	}

	// Remove all bodies and shapes
	for (i = 0; i < BODYPART_COUNT; ++i) {
		m_ownerWorld->removeRigidBody(m_bodies[i]);
		delete m_bodies[i]->getMotionState();
		delete m_bodies[i];
		m_bodies[i] = 0;
		delete m_shapes[i];
		m_shapes[i] = 0;
	}
	delete skeleton;
}

btRigidBody * RagDoll::localCreateRigidBody(btScalar mass, f32 hauteur, f32 rayon,
		const Transform & globalTransformPart, int bodyPart, const Material & mat) {
	// forme graphique
	meshes[bodyPart] = new Capsule(rayon, hauteur, 8, 4);
	objet3Ds[bodyPart] = new Objet3D(mat, meshes[bodyPart], globalTransformPart);
	stringstream tampon;
	tampon << name << bodyPart;
	monde3D->add(tampon.str(), objet3Ds[bodyPart]);

	// forme physique
	m_shapes[bodyPart] = new btCapsuleShape(rayon, hauteur);
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		m_shapes[bodyPart]->calculateLocalInertia(mass, localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, new TransformConv(&(objet3Ds[bodyPart]->getTransform())),
			m_shapes[bodyPart], localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	m_ownerWorld->addRigidBody(body);

	body->setDamping(0.05, 0.85);
	// body->setDeactivationTime(0.8);
	body->setActivationState(DISABLE_DEACTIVATION);
	body->setSleepingThresholds(1.6, 2.5);

	return body;
}

// affichage
void RagDoll::display() const {
	// le dessin des body est fait automatiquement car il on ete ajoute au monde

	// dessiner le squelette de debug
	skeleton->display();

	// calcul de la tranformation du joint dans le repere global
	btTransform jointPereGlobalTransform = m_jointsGeneric6[JOINT_LEFT_HIP]->getCalculatedTransformA();
	btTransform jointFilsGlobalTransform = m_jointsGeneric6[JOINT_LEFT_HIP]->getCalculatedTransformB();

	// calcul de la direction de l'os dans l'animation dans le repere global
	int numFrame = this->skeleton->getNumFrame();
	Vector3 directionCible = this->getTransform().getOrientation() * (skeletonMesh->getOsPosition(numFrame, bodyIndex[BODYPART_LEFT_UPPER_LEG])->fin - skeletonMesh->getOsPosition(numFrame, bodyIndex[BODYPART_LEFT_UPPER_LEG])->debut);
	btTransform jointFilsPereTransform = jointPereGlobalTransform.inverse() * jointFilsGlobalTransform;
	Vector3 directionCourant = TransformConv::btToGraph(jointFilsPereTransform.getRotation()) * Vector3::UNIT_X;
	Quaternion q = directionCourant.getRotationTo(directionCible);
	f32 angles[3];
	Matrix3 m;
	q.ToRotationMatrix(m);
	m.ToEulerAnglesXYZ(angles[0], angles[1], angles[2]);
	cout << angles[0] << ", " << angles[1] << ", " << angles[2] << endl;

	// DEBUT DEBUG
	Transform RagDollGlobalInverseTransform = this->getTransform().inverse();
	Transform jointPereLocalTransform = RagDollGlobalInverseTransform * TransformConv::btToGraph(jointPereGlobalTransform);
	Transform jointFilsLocalTransform = RagDollGlobalInverseTransform * TransformConv::btToGraph(jointFilsGlobalTransform);

	Disable lumiere(GL_LIGHTING);
	Disable texture(GL_TEXTURE_2D);
	Disable profondeur(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	glColor4f(1.0f, 0.0f, 0.0f, 1.f);
	glVertex3fv(jointPereLocalTransform * Vector3::ZERO);
	glVertex3fv(jointPereLocalTransform * Vector3::UNIT_X);
	glVertex3fv(jointFilsLocalTransform * Vector3::ZERO);
	glVertex3fv(jointFilsLocalTransform * Vector3::UNIT_X);
	glColor4f(0.0f, 1.0f, 0.0f, 1.f);
	glVertex3fv(jointPereLocalTransform * Vector3::ZERO);
	glVertex3fv(jointPereLocalTransform * Vector3::UNIT_Y);
	glVertex3fv(jointFilsLocalTransform * Vector3::ZERO);
	glVertex3fv(jointFilsLocalTransform * Vector3::UNIT_Y);
	glColor4f(0.0f, 0.0f, 1.0f, 1.f);
	glVertex3fv(jointPereLocalTransform * Vector3::ZERO);
	glVertex3fv(jointPereLocalTransform * Vector3::UNIT_Z);
	glVertex3fv(jointFilsLocalTransform * Vector3::ZERO);
	glVertex3fv(jointFilsLocalTransform * Vector3::UNIT_Z);
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.f);
}

// fonction de mise a jour du personnage
void RagDoll::update(f32 elapsed) {
	// TODO asservissement
	static f32 vitesse = 1.0;
	static f32 maxMotorForce = 10.8;
	static f32 maxImpulseHinge = 10.8;

	int numFrame = skeleton->getNumFrame();
	// fixer la position du root
	{
		Transform localTransformPart;
		Transform globalTransformRagDoll(this->getTransform().getPosition(), this->getTransform().getOrientation());
		// calcul de la longueur et du rayon de l'os (distance entre les 2 extreminte de l'os)
		Vector3 taille = skeletonMesh->getOsPosition(numFrame, bodyIndex[BODYPART_PELVIS])->fin - skeletonMesh->getOsPosition(
				numFrame, bodyIndex[BODYPART_PELVIS])->debut;
		// calcul de la position du centre de l'os dans le repere de l'animation
		localTransformPart.setPosition((skeletonMesh->getOsPosition(numFrame, bodyIndex[BODYPART_PELVIS])->debut
				+ skeletonMesh->getOsPosition(numFrame, bodyIndex[BODYPART_PELVIS])->fin) / 2.0f);
		// calcul de l'orientation de l'os
		localTransformPart.setRotation(getOrientationOs(taille));
		// creation de la forme physique et graphique
		Transform globalTransformPart = globalTransformRagDoll * localTransformPart;
		// TODO bizare que je dois modifier egalement la position de la physique
		objet3Ds[BODYPART_PELVIS]->getTransform() = globalTransformPart;
		m_bodies[BODYPART_PELVIS]->setCenterOfMassTransform(btTransform(TransformConv::graphToBt(globalTransformPart.getOrientation()), TransformConv::graphToBt(globalTransformPart.getPosition())));
	}

	{
		// asservir la jambe gauche

		// calcul de la tranformation du joint dans le repere global
		btTransform jointPereGlobalTransform = m_jointsGeneric6[JOINT_LEFT_HIP]->getCalculatedTransformA();
		btTransform jointFilsGlobalTransform = m_jointsGeneric6[JOINT_LEFT_HIP]->getCalculatedTransformB();

		btTransform angleTransform = jointPereGlobalTransform.inverse() * jointFilsGlobalTransform;
		f32 angles[3];
		angleTransform.getBasis().getEulerYPR(angles[0], angles[1], angles[2]);
//		cout << angles[0] << ", " << angles[1] << ", " << angles[2] << endl;

		for (int axe = 0; axe < 3; axe++) {
			f32 dif = m_jointsGeneric6[JOINT_LEFT_HIP]->getAngle(axe) - 0.0f;
			dif = -dif;
			m_jointsGeneric6[JOINT_LEFT_HIP]->getRotationalLimitMotor(axe)->m_enableMotor = true;
			m_jointsGeneric6[JOINT_LEFT_HIP]->getRotationalLimitMotor(axe)->m_targetVelocity = dif;
			m_jointsGeneric6[JOINT_LEFT_HIP]->getRotationalLimitMotor(axe)->m_maxMotorForce = maxMotorForce;
		}
	}

	// asservir les autres membres en position 0
	for (int jointId=0; jointId<JOINT_COUNT; jointId++) {
		if (jointId!=JOINT_LEFT_HIP && jointId!=JOINT_RIGHT_HIP) {
		if (CONTRAINTES_IS_CONE[jointId]) {
			for (int axe = 0; axe < 3; axe++) {
				f32 dif = m_jointsGeneric6[jointId]->getAngle(axe);
				dif = -dif *5.0;
				m_jointsGeneric6[jointId]->getRotationalLimitMotor(axe)->m_enableMotor = true;
				m_jointsGeneric6[jointId]->getRotationalLimitMotor(axe)->m_targetVelocity = dif;
				m_jointsGeneric6[jointId]->getRotationalLimitMotor(axe)->m_maxMotorForce = maxMotorForce;
			}
		} else {
			f32 dif = m_jointsHinge[jointId]->getHingeAngle();
			dif = -dif *5.0;
			m_jointsHinge[jointId]->enableAngularMotor(true, dif, maxImpulseHinge);
		}
		}
	}
}
