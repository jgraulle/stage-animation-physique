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

const int RagDoll::BODY_ALIGNE[RagDoll::BODYPART_COUNT][4] = {
		{BODYPART_PELVIS, true, BODYPART_LEFT_UPPER_LEG, true},				// BODYPART_PELVIS
		{BODYPART_SPINE, true, BODYPART_LEFT_UPPER_ARM, true},				// BODYPART_SPINE
		{BODYPART_HEAD, true, BODYPART_HEAD, true},							// BODYPART_HEAD TODO trouver un autre point de repere pour la tete
		{BODYPART_LEFT_LOWER_LEG, true, BODYPART_LEFT_LOWER_LEG, false},	// BODYPART_LEFT_UPPER_LEG
		{BODYPART_LEFT_UPPER_LEG, false, BODYPART_LEFT_UPPER_LEG, true},	// BODYPART_LEFT_LOWER_LEG
		{BODYPART_RIGHT_LOWER_LEG, true, BODYPART_RIGHT_LOWER_LEG, false},	// BODYPART_RIGHT_UPPER_LEG
		{BODYPART_RIGHT_UPPER_LEG, false, BODYPART_RIGHT_UPPER_LEG, true},	// BODYPART_RIGHT_LOWER_LEG
		{BODYPART_LEFT_LOWER_ARM, false, BODYPART_LEFT_LOWER_ARM, true},	// BODYPART_LEFT_UPPER_ARM
		{BODYPART_LEFT_UPPER_ARM, true, BODYPART_LEFT_UPPER_ARM, false},	// BODYPART_LEFT_LOWER_ARM
		{BODYPART_RIGHT_LOWER_ARM, false, BODYPART_RIGHT_LOWER_ARM, true},	// BODYPART_RIGHT_UPPER_ARM
		{BODYPART_RIGHT_UPPER_ARM, true, BODYPART_RIGHT_UPPER_ARM, false}	 // BODYPART_RIGHT_LOWER_ARM
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

const bool RagDoll::CONTRAINTES_IS_CONE[RagDoll::JOINT_COUNT] = {
		false,	// JOINT_PELVIS_SPINE	TODO ajouter un degre de liberte
		true,	// JOINT_SPINE_HEAD
		true,	// JOINT_LEFT_HIP
		false,	// JOINT_LEFT_KNEE
		true,	// JOINT_RIGHT_HIP
		false,	// JOINT_RIGHT_KNEE
		true,	// JOINT_LEFT_SHOULDER
		false,	// JOINT_LEFT_ELBOW
		true,	// JOINT_RIGHT_SHOULDER
		false	// JOINT_RIGHT_ELBOW
};

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
	ostringstream o;
	o << name << "-skelette";
	skeletonName = o.str();
	monde3D->add(skeletonName, skeleton);
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
	// TODO enlever les objet3D du monde graphique
	monde3D->remove(skeletonName);
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
	// le dessin des body et du skelette est fait automatiquement car il on ete ajoute au monde

	Transform RagDollGlobalInverseTransform = this->getTransform().inverse();

	Disable lumiere(GL_LIGHTING);
	Disable texture(GL_TEXTURE_2D);
	Disable profondeur(GL_DEPTH_TEST);
	glBegin(GL_LINES);

	// calcul de la tranformation du joint dans le repere global
	btTransform jointPereInGlobalTransform = m_jointsGeneric6[JOINT_LEFT_HIP]->getCalculatedTransformA();
	btTransform jointFilsInGlobalTransform = m_jointsGeneric6[JOINT_LEFT_HIP]->getCalculatedTransformB();
	btTransform jointFilsInJointPereTrans = jointPereInGlobalTransform.inverse() * jointFilsInGlobalTransform;

	Transform jointPereLocalTransform = RagDollGlobalInverseTransform * TransformConv::btToGraph(jointPereInGlobalTransform);
	Transform jointFilsLocalTransform = RagDollGlobalInverseTransform * TransformConv::btToGraph(jointPereInGlobalTransform * jointFilsInJointPereTrans);

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

Quaternion RagDoll::getOrientationOs(int part) {
	int numFrame = skeleton->getNumFrame();
	// calcul du vecteur des y grace a la direction du membre
	Vector3 vecY =   skeletonMesh->getOsPosition(numFrame, bodyIndex[part])->fin
	               - skeletonMesh->getOsPosition(numFrame, bodyIndex[part])->debut;
	vecY.normalise();
	// calcul du vecteur des x grace a la position du fils
	Vector3 vecDirFils =   skeletonMesh->getOsPosition(numFrame, bodyIndex[BODY_ALIGNE[part][0]], BODY_ALIGNE[part][1])
	                     - skeletonMesh->getOsPosition(numFrame, bodyIndex[BODY_ALIGNE[part][2]], BODY_ALIGNE[part][3]);
	Vector3 vecX = vecY.crossProduct(vecDirFils);
	vecX.normalise();
	// calcul du vecteur des z grace a un produit vectoriel
	Vector3 vecZ = vecX.crossProduct(vecY);
	vecZ.normalise();
	// calcul de la rotation grace au constructeur de quaternion suivant les 3 axes
	Quaternion q;
	if (part==BODYPART_PELVIS || part==BODYPART_SPINE)
		q.FromAxes(-vecZ, vecY, vecX);
	else
		q.FromAxes(vecX, vecY, vecZ);
	return q;
}

// fonction de mise a jour du personnage
void RagDoll::update(f32 elapsed) {
	// TODO asservissement
	static f32 vitesse = 1.0;
	static f32 maxMotorForce = 10.8;
	static f32 maxImpulseHinge = 10.8;

	int numFrame = skeleton->getNumFrameSuivante();
	Transform ragDollInGlobalTrans(this->getTransform().getPosition(), this->getTransform().getOrientation());

	// fixer la position de chaque membre : methode utilisant la position du pere ou fils pour construire un plan
	for (int part=0; part<BODYPART_COUNT; part++) {
		if (   part==BODYPART_PELVIS || part==BODYPART_SPINE
//			|| part==BODYPART_LEFT_UPPER_LEG || part==BODYPART_RIGHT_UPPER_LEG
//		    || part==BODYPART_LEFT_UPPER_ARM || part==BODYPART_RIGHT_UPPER_ARM
//		    || part==BODYPART_LEFT_LOWER_LEG || part==BODYPART_RIGHT_LOWER_LEG
//		    || part==BODYPART_LEFT_LOWER_ARM || part==BODYPART_RIGHT_LOWER_ARM
		   ) {
			// calcul de la position du centre de l'os dans le repere de l'animation
			Vector3 position = (skeletonMesh->getOsPosition(numFrame, bodyIndex[part])->debut
					            + skeletonMesh->getOsPosition(numFrame, bodyIndex[part])->fin) / 2.0f;
			Quaternion orientation = getOrientationOs(part);
			Transform partInRagDollTrans = Transform(position, orientation);
			// positionner la forme graphique
			Transform PartInGlobalTrans = ragDollInGlobalTrans * partInRagDollTrans;
			objet3Ds[part]->getTransform() = PartInGlobalTrans;
			m_bodies[part]->setCenterOfMassTransform(btTransform(TransformConv::graphToBt(PartInGlobalTrans.getOrientation()), TransformConv::graphToBt(PartInGlobalTrans.getPosition())));
		}
	}


	// asservir les articulations avec un seul degrees de liberte
	for (int jointId=0; jointId<JOINT_COUNT; jointId++)	{
		if (   jointId==JOINT_LEFT_KNEE || jointId==JOINT_RIGHT_KNEE
		    || jointId==JOINT_LEFT_ELBOW || jointId==JOINT_RIGHT_ELBOW
		   ) {
			int partPere = CONTRAINTES_BODY[jointId][0];
			int partFils = CONTRAINTES_BODY[jointId][1];
			// calcul du vecteur directeur du membre pere
			Vector3 vecDirPere =   skeletonMesh->getOsPosition(numFrame, bodyIndex[partPere])->fin
			                     - skeletonMesh->getOsPosition(numFrame, bodyIndex[partPere])->debut;
			vecDirPere.normalisef();
			// calcul du vecteur directeur du membre fils
			Vector3 vecDirFils =   skeletonMesh->getOsPosition(numFrame, bodyIndex[partFils])->fin
						         - skeletonMesh->getOsPosition(numFrame, bodyIndex[partFils])->debut;
			vecDirFils.normalisef();
			// calcul de l'angle cible
			f32 angleCible = acosf(vecDirPere.dotProduct(vecDirFils));
			if (vecDirPere.crossProduct(vecDirFils).x<0.0f)
				angleCible = -angleCible;

			// calcul de la difference avec l'angle courant
			f32 dif = angleCible - m_jointsHinge[jointId]->getHingeAngle();
			// ajout d'un correcteur proportionnel
			dif = dif*5.0f;
			// activer l'asservissement en vitesse
			m_jointsHinge[jointId]->enableAngularMotor(true, dif, maxImpulseHinge);
		}
	}

	// asservir les articulations avec 3 degrees de liberte
	for (int jointId=0; jointId<JOINT_COUNT; jointId++)	{
		if (   jointId==JOINT_RIGHT_HIP || jointId==JOINT_LEFT_HIP
			|| jointId==JOINT_RIGHT_SHOULDER || jointId==JOINT_LEFT_SHOULDER
		   ) {
			f32 anglesCourant[3];

	/*		// calcul de l'angle courant methode 1
			btMatrix3x3 jointPereInGlobalOrient = m_jointsGeneric6[jointId]->getCalculatedTransformA().getBasis();
			btMatrix3x3 jointFilsInGlobalOrient = m_jointsGeneric6[jointId]->getCalculatedTransformB().getBasis();
			btMatrix3x3 jointFilsInJointPereOrient = jointPereInGlobalOrient.inverse() * jointFilsInGlobalOrient;
			jointFilsInJointPereOrient.getEulerYPR(anglesCourant[2], anglesCourant[1], anglesCourant[0]);
	*/
	/*		// calcul de l'angle courant methode 2
			btMatrix3x3 partPereInRagDollOrient = m_bodies[CONTRAINTES_BODY[jointId][0]]->getCenterOfMassTransform().getBasis();
			btMatrix3x3 partFilsInRagDollOrient = m_bodies[CONTRAINTES_BODY[jointId][1]]->getCenterOfMassTransform().getBasis();
			btMatrix3x3 jointPereInRagDollOrient = partPereInRagDollOrient * m_jointsGeneric6[jointId]->getFrameOffsetA().getBasis();
			btMatrix3x3 jointFilsInRagDollOrient = partFilsInRagDollOrient * m_jointsGeneric6[jointId]->getFrameOffsetB().getBasis();
			btMatrix3x3 jointFilsInJointPereOrient = jointPereInRagDollOrient.inverse() * jointFilsInRagDollOrient;
			jointFilsInJointPereOrient.getEulerYPR(anglesCourant[2], anglesCourant[1], anglesCourant[0]);
	*/

			// calcul de l'angle courant methode 3
	/*		btMatrix3x3 partPereInRagDollOrient = btMatrix3x3(TransformConv::graphToBt(objet3Ds[CONTRAINTES_BODY[jointId][0]]->getTransform().getOrientation()));
			btMatrix3x3 partFilsInRagDollOrient = btMatrix3x3(TransformConv::graphToBt(objet3Ds[CONTRAINTES_BODY[jointId][1]]->getTransform().getOrientation()));
			btMatrix3x3 jointPereInRagDollOrient = partPereInRagDollOrient * m_jointsGeneric6[jointId]->getFrameOffsetA().getBasis();
			btMatrix3x3 jointFilsInRagDollOrient = partFilsInRagDollOrient * m_jointsGeneric6[jointId]->getFrameOffsetB().getBasis();
			btMatrix3x3 jointFilsInJointPereOrient = jointPereInRagDollOrient.inverse() * jointFilsInRagDollOrient;
			jointFilsInJointPereOrient.getEulerYPR(anglesCourant[2], anglesCourant[1], anglesCourant[0]);
	*/
			// TODO calculer l'angle cible dans le repere du pere
			btMatrix3x3 partPereInRagDollOrient = btMatrix3x3(TransformConv::graphToBt(getOrientationOs(CONTRAINTES_BODY[jointId][0])));
			btMatrix3x3 partFilsInRagDollOrient = btMatrix3x3(TransformConv::graphToBt(getOrientationOs(CONTRAINTES_BODY[jointId][1])));
			btMatrix3x3 jointPereInRagDollOrient = partPereInRagDollOrient * m_jointsGeneric6[jointId]->getFrameOffsetA().getBasis();
			btMatrix3x3 jointFilsInRagDollOrient = partFilsInRagDollOrient * m_jointsGeneric6[jointId]->getFrameOffsetB().getBasis();
			btMatrix3x3 jointFilsInJointPereOrient = jointPereInRagDollOrient.inverse() * jointFilsInRagDollOrient;
			f32 anglesCible[3] = {0.1f, 0.2f, 0.3f};
			jointFilsInJointPereOrient.getEulerYPR(anglesCible[2], anglesCible[1], anglesCible[0]);

			for (int axe = 0; axe < 3; axe++) {
				// calcul de la difference avec l'angle courant
				f32 dif = (-anglesCible[axe]) - m_jointsGeneric6[jointId]->getAngle(axe);
				// ajout d'un correcteur proportionnel
				dif = dif*5.0f;
				// activer l'asservissement en vitesse
				m_jointsGeneric6[jointId]->getRotationalLimitMotor(axe)->m_enableMotor = true;
				m_jointsGeneric6[jointId]->getRotationalLimitMotor(axe)->m_targetVelocity = dif;
				m_jointsGeneric6[jointId]->getRotationalLimitMotor(axe)->m_maxMotorForce = maxMotorForce;
			}
		}
	}


	// asservir les autres membres en position 0
//	for (int jointId=0; jointId<JOINT_COUNT; jointId++) {
	{ int jointId = JOINT_SPINE_HEAD;
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
