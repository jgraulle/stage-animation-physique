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
#include <GL/glfw.h>

const Quaternion RagDoll::QUATERNION_MOINS_UNIT_Y = Quaternion(M_PI, Vector3(0.0f, 0.0f, 1.0f));
const Quaternion RagDoll::QUATERNION_UNIT_Y = Quaternion();

// constante qui definissent l'ensemble des contraintes d'une ragDoll
const int RagDoll::JOINT_TO_BODY[RagDoll::JOINT_COUNT][2] = {
		// Pere, Fils
		{ BODYPART_PELVIS, BODYPART_SPINE },					// JOINT_PELVIS_SPINE
		{ BODYPART_SPINE, BODYPART_HEAD },						// JOINT_SPINE_HEAD
		{ BODYPART_PELVIS, BODYPART_LEFT_UPPER_LEG },			// JOINT_LEFT_HIP
		{ BODYPART_LEFT_UPPER_LEG, BODYPART_LEFT_LOWER_LEG },	// JOINT_LEFT_KNEE
		{ BODYPART_PELVIS, BODYPART_RIGHT_UPPER_LEG },			// JOINT_RIGHT_HIP
		{ BODYPART_RIGHT_UPPER_LEG, BODYPART_RIGHT_LOWER_LEG }, // JOINT_RIGHT_KNEE
		{ BODYPART_SPINE, BODYPART_LEFT_UPPER_ARM },			// JOINT_LEFT_SHOULDER
		{ BODYPART_LEFT_UPPER_ARM, BODYPART_LEFT_LOWER_ARM },	// JOINT_LEFT_ELBOW
		{ BODYPART_SPINE, BODYPART_RIGHT_UPPER_ARM },			// JOINT_RIGHT_SHOULDER
		{ BODYPART_RIGHT_UPPER_ARM, BODYPART_RIGHT_LOWER_ARM }	// JOINT_RIGHT_ELBOW
};

const int RagDoll::BODY_ALIGNE[RagDoll::BODYPART_COUNT][4] = {
		// membreA, ExtremiteA, membreB, ExtremiteB
		{BODYPART_PELVIS, SkeletonMesh::FIN, BODYPART_LEFT_UPPER_LEG, SkeletonMesh::DEBUT},				// BODYPART_PELVIS
		{BODYPART_SPINE, SkeletonMesh::DEBUT, BODYPART_LEFT_UPPER_ARM, SkeletonMesh::DEBUT},			// BODYPART_SPINE
		{BODYPART_HEAD, SkeletonMesh::DEBUT, BODYPART_HEAD, SkeletonMesh::DEBUT},						// BODYPART_HEAD TODO trouver un autre point de repere pour la tete
		{BODYPART_LEFT_LOWER_LEG, SkeletonMesh::DEBUT, BODYPART_LEFT_LOWER_LEG, SkeletonMesh::FIN},		// BODYPART_LEFT_UPPER_LEG
		{BODYPART_LEFT_UPPER_LEG, SkeletonMesh::FIN, BODYPART_LEFT_UPPER_LEG, SkeletonMesh::DEBUT},		// BODYPART_LEFT_LOWER_LEG
		{BODYPART_RIGHT_LOWER_LEG, SkeletonMesh::DEBUT, BODYPART_RIGHT_LOWER_LEG, SkeletonMesh::FIN},	// BODYPART_RIGHT_UPPER_LEG
		{BODYPART_RIGHT_UPPER_LEG, SkeletonMesh::FIN, BODYPART_RIGHT_UPPER_LEG, SkeletonMesh::DEBUT},	// BODYPART_RIGHT_LOWER_LEG
		{BODYPART_LEFT_LOWER_ARM, SkeletonMesh::FIN, BODYPART_LEFT_LOWER_ARM, SkeletonMesh::DEBUT},		// BODYPART_LEFT_UPPER_ARM
		{BODYPART_LEFT_UPPER_ARM, SkeletonMesh::DEBUT, BODYPART_LEFT_UPPER_ARM, SkeletonMesh::FIN},		// BODYPART_LEFT_LOWER_ARM
		{BODYPART_RIGHT_LOWER_ARM, SkeletonMesh::FIN, BODYPART_RIGHT_LOWER_ARM, SkeletonMesh::DEBUT},	// BODYPART_RIGHT_UPPER_ARM
		{BODYPART_RIGHT_UPPER_ARM, SkeletonMesh::DEBUT, BODYPART_RIGHT_UPPER_ARM, SkeletonMesh::FIN}	// BODYPART_RIGHT_LOWER_ARM
};

const btQuaternion RagDoll::JOINT_ORIENTATIONS[RagDoll::JOINT_COUNT][2] = {
		// Pere, fils
		{ btQuaternion(0, 0, 0),			btQuaternion(0, 0, 0) },			// JOINT_PELVIS_SPINE
		{ btQuaternion(0, 0, M_PI_2),		btQuaternion(0, 0, M_PI_2) },		// JOINT_SPINE_HEAD
		{ btQuaternion(0, 0, -M_PI_4 * 1),	btQuaternion(0, 0, -M_PI_4 * 5) },	// JOINT_LEFT_HIP
		{ btQuaternion(M_PI_2,0, 0),		btQuaternion(M_PI_2,0, 0) },		// JOINT_LEFT_KNEE
		{ btQuaternion(0, 0, M_PI_4*5),		btQuaternion(0, 0, M_PI_4) },		// JOINT_RIGHT_HIP
		{ btQuaternion(M_PI_2,0, 0),		btQuaternion(M_PI_2,0, 0) },		// JOINT_RIGHT_KNEE
		{ btQuaternion(0, 0, 0),			btQuaternion(0, 0, M_PI_2) },		// JOINT_LEFT_SHOULDER
		{ btQuaternion(M_PI_2,0, 0),		btQuaternion(M_PI_2,0, 0) },		// JOINT_LEFT_ELBOW
		{ btQuaternion(0, 0, M_PI),			btQuaternion(0, 0, M_PI_2) },		// JOINT_RIGHT_SHOULDER
		{ btQuaternion(M_PI_2,0, 0),		btQuaternion(M_PI_2,0, 0) }			// JOINT_RIGHT_ELBOW
};

const btVector3 RagDoll::JOINT_LIMITES_ANGLES[RagDoll::JOINT_COUNT][2] = {
		// Limite basse,						limite haute
		{ btVector3(-M_PI_2, -M_PI_2, 0),		btVector3(M_PI_4, M_PI_2, 0) },			// JOINT_PELVIS_SPINE
		{ btVector3(-M_PI_4, -M_PI_4, -M_PI_4), btVector3(M_PI_4, M_PI_4, M_PI_4) },	// JOINT_SPINE_HEAD
		{ btVector3(-M_PI_4, -M_PI_4, -M_PI_4), btVector3(M_PI_4, M_PI_4, M_PI_4) },	// JOINT_LEFT_HIP
		{ btVector3(0.0f, 0.0, 0.0),			btVector3(M_PI_2, 0.0, 0.0) },			// JOINT_LEFT_KNEE
		{ btVector3(-M_PI_4, -M_PI_4, -M_PI_4), btVector3(M_PI_4, M_PI_4, M_PI_4) },	// JOINT_RIGHT_HIP
		{ btVector3(0.0f, 0.0, 0.0),			btVector3(M_PI_2, 0.0, 0.0) },			// JOINT_RIGHT_KNEE
		{ btVector3(-M_PI_2, -M_PI_2, -M_PI_2), btVector3(M_PI_2, M_PI_2, M_PI_2) },	// JOINT_LEFT_SHOULDER
		{ btVector3(-M_PI_2, 0.0, 0.0),			btVector3(0.0f, 0.0, 0.0) },			// JOINT_LEFT_ELBOW
		{ btVector3(-M_PI_2, -M_PI_2, -M_PI_2), btVector3(M_PI_2, M_PI_2, M_PI_2) },	// JOINT_RIGHT_SHOULDER
		{ btVector3(-M_PI_2, 0.0, 0.0),			btVector3(0.0f, 0.0, 0.0) }				// JOINT_RIGHT_ELBOW
};

const bool RagDoll::JOINT_IS_CONE[RagDoll::JOINT_COUNT] = {
		true,	// JOINT_PELVIS_SPINE
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

// parcours de la hierarchie des membres
RagDoll::BodyPart RagDoll::jointToBodyPart(RagDoll::Joint joint, Hierarchie hierarchie) {
	return (BodyPart) JOINT_TO_BODY[joint][hierarchie];
}
list<RagDoll::Joint> RagDoll::bodyPartToJoint[BODYPART_COUNT][2];
bool RagDoll::initBodyPartToJoint() {
	// pour chaque joint
	for (Joint joint = JOINT_PELVIS_SPINE; joint < JOINT_COUNT; joint = Joint(joint+1)) {
		// l'ajouter dans la liste
		// de son membre pere en tant que fils
		bodyPartToJoint[jointToBodyPart(joint,PERE)][FILS].push_back(joint);
		// de son membre fils en tant que pere
		bodyPartToJoint[jointToBodyPart(joint,FILS)][PERE].push_back(joint);
	}
/*	cout << "RagDoll::initBodyPartToJoint" << endl;
	for (int part = 0; part < RagDoll::BODYPART_COUNT; part++) {
		cout << "part=" << part << " jointPere=";
		for (list<RagDoll::Joint>::const_iterator it = bodyPartToJoint[part][PERE].begin(); it!=bodyPartToJoint[part][PERE].end(); it++)
			cout << *it << ", ";
		cout << " jointFils=";
		for (list<RagDoll::Joint>::const_iterator it = bodyPartToJoint[part][FILS].begin(); it!=bodyPartToJoint[part][FILS].end(); it++)
			cout << *it << ", ";
		cout << endl;
	}
*/	return true;
}
RagDoll::Joint RagDoll::bodyPartToJointPere(BodyPart part) {
	static bool init = initBodyPartToJoint();
	assert(part!=0);
	return *bodyPartToJoint[part][PERE].begin();
}
const list<RagDoll::Joint> & RagDoll::bodyPartToJointFils(BodyPart part) {
	static bool init = initBodyPartToJoint();
	return bodyPartToJoint[part][FILS];
}
RagDoll::BodyPart RagDoll::getBodyPartPere(BodyPart part) {
	return jointToBodyPart(bodyPartToJointPere(part), PERE);
}
list<RagDoll::BodyPart> RagDoll::bodyPartHierarchie[BODYPART_COUNT];
bool RagDoll::initBodyPartHierarchie() {
	// pour chaque membre sauf le root
	for (BodyPart part = BodyPart(1); part < BODYPART_COUNT; part = BodyPart(part+1)) {
		// l'ajouter dans la liste de son membre pere
		bodyPartHierarchie[getBodyPartPere(part)].push_back(part);
	}
/*	cout << "RagDoll::initBodyPartHierarchie" << endl;
	for (BodyPart part = BODYPART_PELVIS; part < BODYPART_COUNT; part = BodyPart(part+1)) {
		// l'ajouter dans la liste de son membre pere
		cout << "part=" << part << " partFils=";
		for (list<RagDoll::BodyPart>::const_iterator it = bodyPartHierarchie[part].begin(); it!=bodyPartHierarchie[part].end(); it++)
			cout << *it << ", ";
		cout << endl;
	}
	cout << "oki" << endl;
*/	return true;
}
const list<RagDoll::BodyPart> & RagDoll::getBodyPartFils(BodyPart part) {
	static bool init = initBodyPartHierarchie();
	return bodyPartHierarchie[part];
}

// constructeur
RagDoll::RagDoll(const string & name, const SkeletonMesh * bvhFileName, const Material & mat,
		const Transform & transform, btDiscreteDynamicsWorld * m_ownerWorld, Monde3D * monde3D, const string & fileNameExportJoint)
: Objet3D(mat, NULL, transform), name(name), skeletonMesh(bvhFileName), m_ownerWorld(m_ownerWorld), monde3D(monde3D),
temps(0.0), fileNameExportJoint(fileNameExportJoint) {
	assert((transform.getScale().x == transform.getScale().y) && (transform.getScale().y == transform.getScale().z));
	scale = transform.getScale().x;
	inverLocalToGlobal = transform.inverse();

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
		o << "le fichier '" << txtFileName << "' contient " << nbrBodyPart << " partie du corps à la place de "
				<< BODYPART_COUNT << " !";
		throw Erreur(o.str());
	}
	// lire les informations sur chaque partie
	f32 poidsTotal = 0.0f;
	for (int part = 0; part < BODYPART_COUNT; part++) {
		// lire le nom de la partie physique
		if (!(fichier >> line)) {
			ostringstream o;
			o << "erreur de lecture du nom de la part " << part << " dans le fichier '" << txtFileName << "' !";
			throw Erreur(o.str());
		}
		// TODO prendre en consideration le nom du membre

		// lire le nom du joint bvh
		if (!(fichier >> line)) {
			ostringstream o;
			o << "erreur de lecture du nom de la part " << part << " dans le fichier '" << txtFileName << "' !";
			throw Erreur(o.str());
		}
		// convertion du nom en index
		bodyIndexRagDollToSkeleton[part] = skeletonMesh->getJointIndex(line);

		// lire le rayon
		if (!(fichier >> rapportHauteursRayons[part])) {
			ostringstream o;
			o << "erreur de lecture du rayon de la part " << part << " dans le fichier '" << txtFileName << "' !";
			throw Erreur(o.str());
		}
		// lire le poids
		if (!(fichier >> bodyPartPoids[part])) {
			ostringstream o;
			o << "erreur de lecture du poids de la part " << part << " dans le fichier '" << txtFileName << "' !";
			throw Erreur(o.str());
		}

		// calcule du poids total
		poidsTotal += bodyPartPoids[part];
	}

	// creation des objets graphiques et physiques
	Transform localTransformPart;
	f32 hauteur;
	f32 rayon;
	Transform globalTransformRagDoll(transform.getPosition(), transform.getOrientation());
	for (int part = 0; part < BODYPART_COUNT; part++) {
		// calcul de la longueur et du rayon de l'os (distance entre les 2 extreminte de l'os)
		Vector3 taille = skeletonMesh->getOsPosEdition(bodyIndexRagDollToSkeleton[part], SkeletonMesh::FIN)
		               - skeletonMesh->getOsPosEdition(bodyIndexRagDollToSkeleton[part], SkeletonMesh::DEBUT);
		hauteur = taille.length();
		bodyPartHauteurs[part] = hauteur;
		// calcul de la position du centre de l'os dans le repere de l'animation
		localTransformPart.setPosition((skeletonMesh->getOsPosEdition(bodyIndexRagDollToSkeleton[part], SkeletonMesh::DEBUT)
				+ skeletonMesh->getOsPosEdition(bodyIndexRagDollToSkeleton[part], SkeletonMesh::FIN)) / 2.0f);
		// calcul de l'orientation de l'os
		localTransformPart.setRotation(getOrientation(taille));
		// creation de la forme physique et graphique
		Transform globalTransformPart = globalTransformRagDoll * localTransformPart;
		m_bodies[part] = localCreateRigidBody(globalTransformPart, part, mat);
	}

	// creation des jointures
	btTransform pereTrans, filsTrans;
	for (Joint joint = JOINT_PELVIS_SPINE; joint < JOINT_COUNT; joint = Joint(joint+1)) {
		// calcul de la position du point de jointure dans le repere de l'os fils
		filsTrans = btTransform(JOINT_ORIENTATIONS[joint][FILS],
				btVector3(0.0f, -bodyPartHauteurs[jointToBodyPart(joint,FILS)]/2.0f, 0.0f));
		// calculer la position de l'os pere dans le repere de l'os pere par rapport a la position de l'os fils dans le repere de l'os fils
		pereTrans = btTransform(JOINT_ORIENTATIONS[joint][PERE],
				(m_bodies[jointToBodyPart(joint,PERE)]->getCenterOfMassTransform().inverse()(
						m_bodies[jointToBodyPart(joint,FILS)]->getCenterOfMassTransform()(filsTrans.getOrigin()))));
		if (JOINT_IS_CONE[joint]) {
			// contrainte de type epaule, hanche : 3 degres de liberte
			m_jointsGeneric6[joint] = new btGeneric6DofConstraint(*m_bodies[jointToBodyPart(joint,PERE)],
					*m_bodies[jointToBodyPart(joint,FILS)], pereTrans, filsTrans, true);
			m_jointsHinge[joint] = NULL;
			m_joints[joint] = m_jointsGeneric6[joint];
			m_jointsGeneric6[joint]->setAngularLowerLimit(JOINT_LIMITES_ANGLES[joint][JOINT_LIMITES_ANGLES_BASSE]);
			m_jointsGeneric6[joint]->setAngularUpperLimit(JOINT_LIMITES_ANGLES[joint][JOINT_LIMITES_ANGLES_HAUTE]);
			//			m_joints[joint] = new btConeTwistConstraint(*m_bodies[CONTRAINTES_BODY[joint][0]], *m_bodies[CONTRAINTES_BODY[joint][1]], pereTrans, filsTrans);
			//			((btConeTwistConstraint*)m_joints[joint])->setLimit(CONTRAINTES_LIMITES_ANGLES[joint][0], CONTRAINTES_LIMITES_ANGLES[joint][1], CONTRAINTES_LIMITES_ANGLES[joint][2]);
			m_ownerWorld->addConstraint(m_joints[joint], true);
		} else {
			// constrainte de type genoux, coude :  1 seul degres de liberte
			m_jointsHinge[joint] = new btHingeConstraint(*m_bodies[jointToBodyPart(joint,PERE)],
					*m_bodies[jointToBodyPart(joint,FILS)], pereTrans, filsTrans);
			m_jointsGeneric6[joint] = NULL;
			m_joints[joint] = m_jointsHinge[joint];
			m_jointsHinge[joint]->setLimit(JOINT_LIMITES_ANGLES[joint][JOINT_LIMITES_ANGLES_BASSE][0], JOINT_LIMITES_ANGLES[joint][JOINT_LIMITES_ANGLES_HAUTE][0]);
			m_ownerWorld->addConstraint(m_joints[joint], true);
		}
	}

	// creation du squelette de debug
	skeleton = new Skeleton(skeletonMesh, mat, transform);
	ostringstream o;
	o << name << "-skelette";
	skeletonName = o.str();
	monde3D->add(skeletonName, skeleton);

	// calcul de la hauteur totale : coordonnee y de l'extremite fin de la tete - coordonnee y de l'extremite fin de la jambe
	f32 hauteurTotale = m_bodies[BODYPART_HEAD]->getCenterOfMassPosition().getY() + bodyPartHauteurs[BODYPART_HEAD]
					  - (m_bodies[BODYPART_RIGHT_LOWER_LEG]->getCenterOfMassPosition().getY() - bodyPartHauteurs[BODYPART_RIGHT_LOWER_LEG]);
	cout << "creation du personnage '" << name << "' de taille " << hauteurTotale << " et de poids " << poidsTotal << endl;

	// si sauvegarde du mouvement
	if (fileNameExportJoint!="") {
		// entete sauvegarde mouvement
		for (Joint jointId = Joint(0); jointId<JOINT_COUNT; jointId = Joint(jointId+1)) {
			ostringstream buf;
			buf << "data/" << fileNameExportJoint << jointId << ".csv";
			fileExportJoint[jointId].open(buf.str().c_str());
			fileExportJoint[jointId] << "temps; ";
			// asservir les articulations avec 3 degrees de liberte
			if (   jointId==JOINT_RIGHT_HIP || jointId==JOINT_LEFT_HIP
				|| jointId==JOINT_RIGHT_SHOULDER || jointId==JOINT_LEFT_SHOULDER
				|| jointId==JOINT_PELVIS_SPINE
			   ) {
				for (char axe = 'X'; axe<='Z'; axe++) {
					fileExportJoint[jointId] << "angle" << axe << "; ";
				}
				for (char axe = 'X'; axe<='Z'; axe++) {
					fileExportJoint[jointId] << "angleCible" << axe << "; ";
				}
			// asservir les articulations avec un seul degrees de liberte
			} else if (   jointId==JOINT_LEFT_KNEE || jointId==JOINT_RIGHT_KNEE
				|| jointId==JOINT_LEFT_ELBOW || jointId==JOINT_RIGHT_ELBOW
			   ) {
				fileExportJoint[jointId] << "angle" << "; ";
				fileExportJoint[jointId] << "angleCible" << "; ";
			}
			for (char axe = 'X'; axe<='Z'; axe++) {
				fileExportJoint[jointId] << "position" << axe << "; ";
			}
			for (char axe = 'X'; axe<='Z'; axe++) {
				fileExportJoint[jointId] << "positionCible" << axe << "; ";
			}
			fileExportJoint[jointId] << endl;
		}
	}
}

// destructeur
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

// outils de creations
btRigidBody * RagDoll::localCreateRigidBody(const Transform & globalTransformPart, int bodyPart, const Material & mat) {
	btScalar mass = bodyPartPoids[bodyPart];
	f32 hauteur = bodyPartHauteurs[bodyPart];
	f32 rayon = hauteur / rapportHauteursRayons[bodyPart];
	// forme graphique
	meshes[bodyPart] = new Capsule(rayon, hauteur, 8, 4);
	objet3Ds[bodyPart] = new Objet3D(mat, meshes[bodyPart], globalTransformPart);
	stringstream tampon;
	tampon << name << bodyPart;
	monde3D->add(tampon.str(), objet3Ds[bodyPart]);

	// forme physique
	// TODO faire des pieds plat
//	if (bodyPart!=BODYPART_LEFT_LOWER_LEG)
		m_shapes[bodyPart] = new btCapsuleShape(rayon, hauteur);
//	else
//		m_shapes[bodyPart] = new btCylinderShape(btVector3(rayon, hauteur/2.0, rayon));

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
//	body->setActivationState(DISABLE_DEACTIVATION);
	body->setSleepingThresholds(1.6, 2.5);
	body->setFriction(3.0f);

	return body;
}

// affichage
void RagDoll::display() const {
	// le dessin des body et du skelette est fait automatiquement car il on ete ajoute au monde


	Disable lumiere(GL_LIGHTING);
	Disable texture(GL_TEXTURE_2D);
	Disable profondeur(GL_DEPTH_TEST);
	glPointSize(3.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.f);
	glBegin(GL_LINES);

	f32 scaleRepere = 0.1f;
	int numFrame = skeleton->getNumFrameSuivante();
/*
	// afficher le repere du ragdoll
	Transform centreRagdollToLocalTrans = inverLocalToGlobal * objet3Ds[BODYPART_PELVIS]->getTransform();
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3fv(centreRagdollToLocalTrans * Vector3::ZERO);
	glVertex3fv(centreRagdollToLocalTrans * (Vector3::UNIT_X * scaleRepere));
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3fv(centreRagdollToLocalTrans * Vector3::ZERO);
	glVertex3fv(centreRagdollToLocalTrans * (Vector3::UNIT_Y * scaleRepere));
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3fv(centreRagdollToLocalTrans * Vector3::ZERO);
	glVertex3fv(centreRagdollToLocalTrans * (Vector3::UNIT_Z * scaleRepere));

	// afficher les os
	for (Joint joint = JOINT_PELVIS_SPINE; joint < JOINT_COUNT; joint = Joint(joint+1)) {
		if (   joint==JOINT_LEFT_ELBOW
//		    || joint==JOINT_RIGHT_KNEE
		   )
		{
			// os pere
			Transform osPereToLocalTrans = inverLocalToGlobal * objet3Ds[jointToBodyPart(joint,PERE)]->getTransform();
			glColor4f(1.0f, 1.0f, 1.0f, 1.f);
			glVertex3fv(osPereToLocalTrans * Vector3(0.0f, bodyPartHauteurs[jointToBodyPart(joint,PERE)]/2.0f, 0.0f));
			glColor4f(1.0f, 1.0f, 1.0f, 1.f);
			glVertex3fv(osPereToLocalTrans * Vector3(0.0f, -bodyPartHauteurs[jointToBodyPart(joint,PERE)]/2.0f, 0.0f));
			// os fils
			Transform osFilsToLocalTrans = inverLocalToGlobal * objet3Ds[jointToBodyPart(joint,FILS)]->getTransform();
			glColor4f(1.0f, 1.0f, 1.0f, 1.f);
			glVertex3fv(osFilsToLocalTrans * Vector3(0.0f, bodyPartHauteurs[jointToBodyPart(joint,FILS)]/2.0f, 0.0f));
			glColor4f(1.0f, 1.0f, 1.0f, 1.f);
			glVertex3fv(osFilsToLocalTrans * Vector3(0.0f, -bodyPartHauteurs[jointToBodyPart(joint,FILS)]/2.0f, 0.0f));
		}
	}


	// affichage des reperes des membres
	Transform membrePereToLocalTrans, membreFilsToLocalTrans;
	for (BodyPart part = BodyPart(0);
	part < BODYPART_COUNT;
	part = BodyPart(part+1)) {
		if (  part==BODYPART_LEFT_UPPER_ARM
		   || part==BODYPART_LEFT_LOWER_ARM
		   )
		{
			membrePereToLocalTrans = inverLocalToGlobal * objet3Ds[part]->getTransform();
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glVertex3fv(membrePereToLocalTrans * Vector3::ZERO);
			glVertex3fv(membrePereToLocalTrans * (Vector3::UNIT_X*scaleRepere));
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glVertex3fv(membrePereToLocalTrans * Vector3::ZERO);
			glVertex3fv(membrePereToLocalTrans * (Vector3::UNIT_Y*scaleRepere));
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glVertex3fv(membrePereToLocalTrans * Vector3::ZERO);
			glVertex3fv(membrePereToLocalTrans * (Vector3::UNIT_Z*scaleRepere));
		}
	}

	// affichage des reperes des jointures
	btTransform jointToOsPereTrans, jointToOsFilsTrans;
	Transform osFilsToLocalTrans, osPereToLocalTrans, jointPereToLocalTrans, jointFilsToLocalTrans;
	for (Joint joint = Joint(0); joint < JOINT_COUNT; joint = Joint(joint+1)) {
		if (  joint==JOINT_LEFT_ELBOW
		// || joint==JOINT_RIGHT_KNEE
		   )
		{
			// calcul de la position du point de jointure dans le repere de l'os pere
			// calcul de la position du point de jointure dans le repere de l'os fils
			if (JOINT_IS_CONE[joint])
				jointToOsPereTrans = m_jointsGeneric6[joint]->getFrameOffsetA();
			else
				jointToOsPereTrans = m_jointsHinge[joint]->getAFrame();
			osPereToLocalTrans = inverLocalToGlobal * objet3Ds[jointToBodyPart(joint,PERE)]->getTransform();
			jointPereToLocalTrans = osPereToLocalTrans * TransformConv::btToGraph(jointToOsPereTrans);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glVertex3fv(jointPereToLocalTrans * Vector3::ZERO);
			glVertex3fv(jointPereToLocalTrans * (Vector3::UNIT_X*scaleRepere));
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glVertex3fv(jointPereToLocalTrans * Vector3::ZERO);
			glVertex3fv(jointPereToLocalTrans * (Vector3::UNIT_Y*scaleRepere));
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glVertex3fv(jointPereToLocalTrans * Vector3::ZERO);
			glVertex3fv(jointPereToLocalTrans * (Vector3::UNIT_Z*scaleRepere));

			// calcul de la position du point de jointure dans le repere de l'os fils
			if (JOINT_IS_CONE[joint])
				jointToOsFilsTrans = m_jointsGeneric6[joint]->getFrameOffsetB();
			else
				jointToOsFilsTrans = m_jointsHinge[joint]->getBFrame();
			osFilsToLocalTrans = inverLocalToGlobal * objet3Ds[jointToBodyPart(joint,FILS)]->getTransform();
			jointFilsToLocalTrans = osFilsToLocalTrans * TransformConv::btToGraph(jointToOsFilsTrans);
			glColor4f(0.5f, 0.0f, 0.0f, 1.0f);
			glVertex3fv(jointFilsToLocalTrans * Vector3::ZERO);
			glVertex3fv(jointFilsToLocalTrans * (Vector3::UNIT_X*scaleRepere));
			glColor4f(0.0f, 0.5f, 0.0f, 1.0f);
			glVertex3fv(jointFilsToLocalTrans * Vector3::ZERO);
			glVertex3fv(jointFilsToLocalTrans * (Vector3::UNIT_Y*scaleRepere));
			glColor4f(0.0f, 0.0f, 0.5f, 1.0f);
			glVertex3fv(jointFilsToLocalTrans * Vector3::ZERO);
			glVertex3fv(jointFilsToLocalTrans * (Vector3::UNIT_Z*scaleRepere));
		}
	}

*/
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.f);
	glPointSize(1.0f);
}

// outils d'orientation des os
Quaternion RagDoll::getOrientation(const Vector3 & v) {
	Quaternion rotations;
	if ((v.x < -1e-06 || v.x > 1e-06) || (v.z < -1e-06 || v.z > 1e-06))
		return Vector3::UNIT_Y.getRotationTo(v);
	// else {
	if (v.y < 0.0f)
		return QUATERNION_MOINS_UNIT_Y;
	//else
	return QUATERNION_UNIT_Y;
	//}
}
Quaternion RagDoll::getOrientationOs(int part, int numFrame) {
	// calcul du vecteur des y grace a la direction du membre
	Vector3 vecY =   skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[part], SkeletonMesh::FIN)
	               - skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[part], SkeletonMesh::DEBUT);
	vecY.normalise();
	// calcul du vecteur des x grace a la position du fils
	Vector3 vecDirFils =   skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[BODY_ALIGNE[part][0]], SkeletonMesh::Extremite(BODY_ALIGNE[part][1]))
						 - skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[BODY_ALIGNE[part][2]], SkeletonMesh::Extremite(BODY_ALIGNE[part][3]));
	// cas particulier du torse car il est oriente par rapport a la moyenne des positions des epaules
	if (part==BODYPART_SPINE) {
		// on prend la moyenne des vecteurs forme par les clavicules
		vecDirFils +=    skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[BODY_ALIGNE[part][0]], SkeletonMesh::Extremite(BODY_ALIGNE[part][1]))
		               - skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[BODY_ALIGNE[part][2]+BODYPART_LEFT_TO_RIGHT], SkeletonMesh::Extremite(BODY_ALIGNE[part][3]));
		vecDirFils /= -2.0f;
	}
	Vector3 vecX = vecY.crossProduct(vecDirFils);
	vecX.normalise();
	vecX.getRotationTo(vecY);
	// calcul du vecteur des z grace a un produit vectoriel
	Vector3 vecZ = vecX.crossProduct(vecY);
	vecZ.normalise();
	// calcul de la rotation grace au constructeur de quaternion suivant les 3 axes
	Quaternion q;
	// cas particulier du bassin dont le plan forme par le 3eme point (la hanche) est a 90° selon l'axe des Y
	if (part==BODYPART_PELVIS)
		q.FromAxes(-vecZ, vecY, vecX);
	else
		q.FromAxes(vecX, vecY, vecZ);
	return q;
}

// fonction de mise a jour du personnage
void RagDoll::update(f32 elapsed) {
	// TODO regler le pid
	static f32 maxMotorForce = 2.5;
	static f32 maxImpulseHinge = 2.5;

	int numFrame = skeleton->getNumFrameSuivante();
	Transform ragDollInGlobalTrans(this->getTransform().getPosition(), this->getTransform().getOrientation());

	// TODO tenir debou
	btVector3 penche = m_bodies[BODYPART_SPINE]->getCenterOfMassPosition() - m_bodies[BODYPART_PELVIS]->getCenterOfMassPosition();
	m_bodies[BODYPART_HEAD]->applyCentralForce(btVector3(penche.getX(),0,penche.getZ()) * -40000.0);
//	m_bodies[BODYPART_PELVIS]->applyCentralForce(btVector3(penche.getX(),0,penche.getZ()) * -40000.0);

	// fixer la position de chaque membre : methode utilisant la position du pere ou fils pour construire un plan
	// TODO faille quand les 3 points sont alignes
/*	for (int part=0; part<BODYPART_COUNT; part++) {
		if (   part==BODYPART_PELVIS
//			|| part==BODYPART_SPINE
//			|| part==BODYPART_LEFT_UPPER_LEG || part==BODYPART_RIGHT_UPPER_LEG
//		    || part==BODYPART_LEFT_UPPER_ARM || part==BODYPART_RIGHT_UPPER_ARM
//		    || part==BODYPART_LEFT_LOWER_LEG || part==BODYPART_RIGHT_LOWER_LEG
//		    || part==BODYPART_LEFT_LOWER_ARM || part==BODYPART_RIGHT_LOWER_ARM
		   ) {
			// calcul de la position du centre de l'os dans le repere de l'animation
			Vector3 position = (skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[part], SkeletonMesh::DEBUT)
					            + skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[part], SkeletonMesh::FIN)) / 2.0f;
			Quaternion orientation = getOrientationOs(part, numFrame);
			Transform partInRagDollTrans = Transform(position, orientation);
			// positionner la forme graphique
			Transform PartInGlobalTrans = ragDollInGlobalTrans * partInRagDollTrans;
			objet3Ds[part]->getTransform() = PartInGlobalTrans;
			m_bodies[part]->setCenterOfMassTransform(btTransform(TransformConv::graphToBt(PartInGlobalTrans.getOrientation()), TransformConv::graphToBt(PartInGlobalTrans.getPosition())));
		}
	}
*/
	// asservir les articulations
	for (Joint jointId = JOINT_PELVIS_SPINE; jointId < JOINT_COUNT; jointId = Joint(jointId+1)) {
		// si sauvegarde du mouvement
		if (fileNameExportJoint!="") {
			fileExportJoint[jointId] << glfwGetTime() << "; ";
		}
		// asservir les articulations avec 3 degrees de liberte
		if (   jointId==JOINT_RIGHT_HIP || jointId==JOINT_LEFT_HIP
			|| jointId==JOINT_RIGHT_SHOULDER || jointId==JOINT_LEFT_SHOULDER
			|| jointId==JOINT_PELVIS_SPINE
		   ) {
			// calcul des angles cibles : du joint du fils dans le repere du joint du pere
			btMatrix3x3 partPereInRagDollOrient = btMatrix3x3(TransformConv::graphToBt(getOrientationOs(jointToBodyPart(jointId,PERE), numFrame)));
			btMatrix3x3 partFilsInRagDollOrient = btMatrix3x3(TransformConv::graphToBt(getOrientationOs(jointToBodyPart(jointId,FILS), numFrame)));
			btMatrix3x3 jointPereInRagDollOrient = partPereInRagDollOrient * m_jointsGeneric6[jointId]->getFrameOffsetA().getBasis();
			btMatrix3x3 jointFilsInRagDollOrient = partFilsInRagDollOrient * m_jointsGeneric6[jointId]->getFrameOffsetB().getBasis();
			btMatrix3x3 jointFilsInJointPereOrient = jointPereInRagDollOrient.inverse() * jointFilsInRagDollOrient;
			f32 anglesCible[3];
			jointFilsInJointPereOrient.getEulerYPR(anglesCible[2], anglesCible[1], anglesCible[0]);

			for (int axe = 0; axe < 3; axe++) {
				// calcul de la difference avec l'angle courant
				f32 dif = (-anglesCible[axe]) - m_jointsGeneric6[jointId]->getAngle(axe);
				// ajout d'un correcteur proportionnel
				dif = dif*20.0f;
				// activer l'asservissement en vitesse
				m_jointsGeneric6[jointId]->getRotationalLimitMotor(axe)->m_enableMotor = true;
				m_jointsGeneric6[jointId]->getRotationalLimitMotor(axe)->m_targetVelocity = dif;
				m_jointsGeneric6[jointId]->getRotationalLimitMotor(axe)->m_maxMotorForce = maxMotorForce;
			}
			// si sauvegarde du mouvement
			if (fileNameExportJoint!="") {
				fileExportJoint[jointId] << m_jointsGeneric6[jointId]->getAngle(0) << "; ";
				fileExportJoint[jointId] << m_jointsGeneric6[jointId]->getAngle(1) << "; ";
				fileExportJoint[jointId] << m_jointsGeneric6[jointId]->getAngle(2) << "; ";
				fileExportJoint[jointId] << -anglesCible[0] << "; ";
				fileExportJoint[jointId] << -anglesCible[1] << "; ";
				fileExportJoint[jointId] << -anglesCible[2] << "; ";
				Vector3 pos = inverLocalToGlobal * TransformConv::btToGraph(m_jointsGeneric6[jointId]->getCalculatedTransformA().getOrigin());
				fileExportJoint[jointId] << pos[0] << "; ";
				fileExportJoint[jointId] << pos[1] << "; ";
				fileExportJoint[jointId] << pos[2] << "; ";
			}
		}
		// asservir les articulations avec un seul degrees de liberte
		else if (   jointId==JOINT_LEFT_KNEE || jointId==JOINT_RIGHT_KNEE
		    || jointId==JOINT_LEFT_ELBOW || jointId==JOINT_RIGHT_ELBOW
		   ) {
			int partPere = jointToBodyPart(jointId,PERE);
			int partFils = jointToBodyPart(jointId,FILS);

			// calcul de l'angle cible : du joint du fils dans le repere du joint du pere
			btMatrix3x3 partPereInRagDollOrient = btMatrix3x3(TransformConv::graphToBt(getOrientationOs(partPere, numFrame)));
			btMatrix3x3 partFilsInRagDollOrient = btMatrix3x3(TransformConv::graphToBt(getOrientationOs(partFils, numFrame)));
			btMatrix3x3 jointPereInRagDollOrient = partPereInRagDollOrient * m_jointsHinge[jointId]->getAFrame().getBasis();
			btMatrix3x3 jointFilsInRagDollOrient = partFilsInRagDollOrient * m_jointsHinge[jointId]->getBFrame().getBasis();
			btMatrix3x3 jointFilsInJointPereOrient = jointPereInRagDollOrient.inverse() * jointFilsInRagDollOrient;
			f32 anglesCible[3];
			jointFilsInJointPereOrient.getEulerYPR(anglesCible[2], anglesCible[1], anglesCible[0]);
			f32 angleCible = -anglesCible[2];

/*			// TODO optimiser en reglant le bug mesure de l'angle avec 2eme skelette et translation root = true
			// calcul du vecteur directeur du membre pere
			Vector3 vecDirPere =   skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[partPere], SkeletonMesh::FIN)
			                     - skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[partPere], SkeletonMesh::DEBUT);
			vecDirPere.normalisef();
			// calcul du vecteur directeur du membre fils
			Vector3 vecDirFils =   skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[partFils], SkeletonMesh::FIN)
						         - skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[partFils], SkeletonMesh::DEBUT);
			vecDirFils.normalisef();

			// calcul de l'angle cible : methode 2
			f32 dotProduct = vecDirPere.dotProduct(vecDirFils);
			f32 angleCible = acosf(dotProduct);
			if (vecDirPere.crossProduct(vecDirFils).x<0.0f)
				angleCible = -angleCible;
*/
			// si sauvegarde du mouvement
			if (fileNameExportJoint!="") {
				fileExportJoint[jointId] << m_jointsHinge[jointId]->getHingeAngle() << "; ";
				fileExportJoint[jointId] << angleCible << "; ";
				Vector3 pos = inverLocalToGlobal * TransformConv::btToGraph((m_jointsHinge[jointId]->getRigidBodyA().getCenterOfMassTransform() * m_jointsHinge[jointId]->getAFrame()).getOrigin());
				fileExportJoint[jointId] << pos[0] << "; ";
				fileExportJoint[jointId] << pos[1] << "; ";
				fileExportJoint[jointId] << pos[2] << "; ";
			}
			// calcul de la difference avec l'angle courant
			f32 dif = angleCible - m_jointsHinge[jointId]->getHingeAngle();
			// ajout d'un correcteur proportionnel
			dif = dif*20.0f;
			// activer l'asservissement en vitesse
			m_jointsHinge[jointId]->enableAngularMotor(true, dif, maxImpulseHinge);
		}
		// asservir en angle 0
		else if (jointId==JOINT_SPINE_HEAD
//				|| jointId==JOINT_PELVIS_SPINE
			) {
			if (JOINT_IS_CONE[jointId]) {
				for (int axe = 0; axe < 3; axe++) {
					f32 dif = 0.0f - m_jointsGeneric6[jointId]->getAngle(axe);
					dif = dif *5.0;
					m_jointsGeneric6[jointId]->getRotationalLimitMotor(axe)->m_enableMotor = true;
					m_jointsGeneric6[jointId]->getRotationalLimitMotor(axe)->m_targetVelocity = dif;
					m_jointsGeneric6[jointId]->getRotationalLimitMotor(axe)->m_maxMotorForce = maxMotorForce;
				}
			} else {
				f32 dif = 0.0f - m_jointsHinge[jointId]->getHingeAngle();
				dif = dif * 5.0;
				m_jointsHinge[jointId]->enableAngularMotor(true, dif, maxImpulseHinge);
			}
		}
		// si sauvegarde du mouvement
		if (fileNameExportJoint!="") {
			fileExportJoint[jointId] << skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[jointToBodyPart(jointId,FILS)], SkeletonMesh::DEBUT)[0] << "; ";
			fileExportJoint[jointId] << skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[jointToBodyPart(jointId,FILS)], SkeletonMesh::DEBUT)[1] << "; ";
			fileExportJoint[jointId] << skeletonMesh->getOsPosition(numFrame, bodyIndexRagDollToSkeleton[jointToBodyPart(jointId,FILS)], SkeletonMesh::DEBUT)[2] << "; ";
			fileExportJoint[jointId] << endl;
		}
	}
}

// acceder a la position courante d'un membre (son centre et ses extremites)
Transform RagDoll::getLocalTransformPart(BodyPart part) {
	return inverLocalToGlobal * objet3Ds[(int)part]->getTransform();
}
Transform RagDoll::getLocalTransformPart(BodyPart part, SkeletonMesh::Extremite extremite) {
	int sens = extremite*2-1;
	return inverLocalToGlobal * objet3Ds[(int)part]->getTransform() * Transform(Vector3(0.0f, sens * bodyPartHauteurs[(int)part]/2.0f, 0.0f));
}
