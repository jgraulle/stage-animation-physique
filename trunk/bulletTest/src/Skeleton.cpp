/*
 * Perso.cpp
 *
 *  Created on: 26 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Skeleton.h"

Skeleton::Skeleton(const string & bvhFileName, Material material, Transform transform, Quaternion orientationEdition, f32 scale)
: Objet3D(material, NULL, transform), bvhFileName(bvhFileName), tempsAnim(0.0), numFrame(0), orientationEdition(orientationEdition), scale(scale) {
	// chargement du fichier bvh
	if(motion_load_bvh(&motion, bvhFileName.c_str()) < 0)
		throw ErreurFileNotFound(bvhFileName, "");
	// initialisation de la structure frame
	frame = motion_frame_init(NULL, motion);
	// calculer le temps d'une frame
	motion_get_frame(frame, motion, 1);
	tempsParFrame = motion_frame_get_time(frame);
	// acceder au nombre de joint
	nbrJoints = motion_frame_get_joints_n(frame);
	// retourner a la 1er frame
	motion_get_frame(frame, motion, 0);
	// ajouter les noms de chaque joint
	for (int joinId=0; joinId<nbrJoints; joinId++) {
		// pour tous les fils
		string nom = joint_get_name(frame, joinId);
		// chercher ce nom dans la table de hash
		NameIndex::iterator itn = jointsName.find(nom);
		// si cet objet n'existe pas deja
		if (itn == jointsName.end())
			jointsName.insert(make_pair(nom, joinId));
		else
			// afficher un message d'avertissement
			cout << "attention : le joint '" + nom + "' est en double dans le fichier '" + bvhFileName + "'" <<  endl;
	}
	// dimentionner le tableau des joints a la bonne taille
	jointsPos.resize(motion_frame_get_joints_n(frame), Vector3::ZERO);
	jointsPosEdition.resize(motion_frame_get_joints_n(frame), NULL);
	// calculer la position des joints en edition
	rootId = motion_frame_get_root_joint(frame);
	if (rootId==-1)
		throw Erreur("le fichier '"+bvhFileName+"' n'a pas de root !");
	jointPosEdition(rootId, Vector3::ZERO);
}

Skeleton::~Skeleton() {
	motion_frame_free(frame, true);
	motion_free(motion, true);
}

void Skeleton::jointPosEdition(int joinId, const Vector3 & accumulateur) {
	int childId = joint_get_child(frame, joinId);
	int nbrChild = 0;
	Vector3 offset, moyenne = Vector3(0.0f, 0.0f, 0.0f);
	while (childId != -1) {
		nbrChild++;
		// lire la position de ce joint
		joint_get_offset(frame, childId, offset);
		offset = orientationEdition * offset * scale;

		moyenne += accumulateur + offset;

		// afficher tous les petits fils
		jointPosEdition(childId, accumulateur + offset);

		// passer au fils suivant
		childId = joint_get_next(frame, childId);
	}

	if (nbrChild >= 1) {
		jointsPosEdition[joinId] = new Os(accumulateur, moyenne / nbrChild, joinId);
	}
}

// affichage
void Skeleton::display() const {
	Disable lumiere(GL_LIGHTING);
	Disable texture(GL_TEXTURE_2D);
	Disable profondeur(GL_DEPTH_TEST);
	glPointSize(3.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.f);
	glBegin(GL_POINTS);

	for (int joinId=0; joinId<nbrJoints; joinId++)
		glVertex3fv(jointsPos[joinId]);

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.f);
	glPointSize(1.0f);
	glBegin(GL_LINES);
	for (int joinId=0; joinId<nbrJoints; joinId++) {
		if (jointsPosEdition[joinId]!=NULL) {
			glVertex3fv(jointsPosEdition[joinId]->debut);
			glVertex3fv(jointsPosEdition[joinId]->fin);
		}
	}
	glEnd();
}

// fonction de mise a jour de l'objet
void Skeleton::update(f32 elapsed) {
	tempsAnim += elapsed;
	// si le temps est plus grand que le temps de debut de la prochaine frame
	if (tempsAnim>motion_frame_get_time(frame)+tempsParFrame) {
		// passer a la frame suivante
		numFrame++;
		if (motion_get_frame(frame, motion, numFrame)!=0) {
			// repartir a la frame 0 si derniere frame
			numFrame = 0;
			tempsAnim = 0.0;
			motion_get_frame(frame, motion, numFrame);
		}
		// mettre a jour le tableau des os : pour tous les fils du root
		int childId = joint_get_child(frame, rootId);
		while (childId!=-1) {
			// afficher tous les petits fils
			updateJoin(childId, Transform(Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE));
			// passer au fils suivant
			childId = joint_get_next(frame, childId);
		}
	}
}

void Skeleton::updateJoin(int joinId, const Transform & transParent) {
	Vector3 temp, position = Vector3::ZERO;
	Quaternion orientation = Quaternion::IDENTITY;
	int bind[3];

	// acces a la position
	joint_get_offset(frame, joinId, position);
	joint_get_position(frame, joinId, temp);
	position += temp;

	// acces a l'orientation
	joint_get_orientation(frame, joinId, temp, bind);
	for (int i=0; i<3; i++) {
		switch(bind[i]) {
		case JOINT_XROT:
			orientation = orientation * Quaternion(temp[i] * M_PI / 180.0f, Vector3::UNIT_X);
			break;
		case JOINT_YROT:
			orientation = orientation * Quaternion(temp[i] * M_PI / 180.0f, Vector3::UNIT_Y);
			break;
		case JOINT_ZROT:
			orientation = orientation * Quaternion(temp[i] * M_PI / 180.0f, Vector3::UNIT_Z);
			break;
		}
	}

	// application de la transformation
	Transform global;
	Transform local(position, orientation);
	global = transParent * local;

	// enregistrer la position du joint
	jointsPos[joinId] = orientationEdition * (global * Vector3::ZERO) * scale;

	// pour tous les fils
	int childId = joint_get_child(frame, joinId);
	while (childId!=-1) {
		// afficher tous les petits fils
		updateJoin(childId, global);
		// passer au fils suivant
		childId = joint_get_next(frame, childId);
	}
}

const Vector3 & Skeleton::getJointPosition(int joinId) const {
	return jointsPos[joinId];
}

const char * Skeleton::getJointName(int joinId) const {
	return joint_get_name(frame, joinId);
}
int Skeleton::getJointIndex(const string & name) const {
	// chercher ce nom dans la table de hash
	NameIndex::const_iterator itn = jointsName.find(name);
	// si cet objet n'existe pas
	if (itn == jointsName.end())
		throw Erreur("le joint '"+name+"' n'existe pas dans le squelette '"+bvhFileName+"' !");
	// else
	return itn->second;
}

