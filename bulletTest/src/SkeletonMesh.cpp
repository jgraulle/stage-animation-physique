/*
 * SkeletonMesh.cpp
 *
 *  Created on: 2 avril 2009
 *      Author: jeremie GRAULLE
 */

#include "SkeletonMesh.h"

SkeletonMesh::SkeletonMesh(const string & bvhFileName, Quaternion orientationEdition, f32 scale)
: bvhFileName(bvhFileName), orientationEdition(orientationEdition), scale(scale) {
	// chargement du fichier bvh
	MOTION * motion;
	if(motion_load_bvh(&motion, bvhFileName.c_str()) < 0)
		throw ErreurFileNotFound(bvhFileName, "");
	// initialisation de la structure frame
	MFRAME * frame;
	frame = motion_frame_init(NULL, motion);
	// calculer le temps d'une frame
	motion_get_frame(frame, motion, 1);
	tempsParFrame = motion_frame_get_time(frame);
	motion_get_frame(frame, motion, 0);
	// acceder a la duree totale de l'animation
	dureeTotalAnimation = motion_get_time(motion);
	// calculer le nombre totale de frame
	nbrTotalFrames = (int)(dureeTotalAnimation/tempsParFrame);
	// acceder au nombre de joint
	nbrJoints = motion_frame_get_joints_n(frame);

	// ajouter les noms de chaque joint
	jointsNameById.resize(nbrJoints);
	for (int joinId=0; joinId<nbrJoints; joinId++) {
		// pour tous les fils
		string nom = joint_get_name(frame, joinId);
		jointsNameById[joinId] = nom;
		// chercher ce nom dans la table de hash
		NameIndex::iterator itn = jointsName.find(nom);
		// si cet objet n'existe pas deja
		if (itn == jointsName.end())
			jointsName.insert(make_pair(nom, joinId));
		else
			// afficher un message d'avertissement
			cout << "attention : le joint '" + nom + "' est en double dans le fichier '" + bvhFileName + "'" <<  endl;
	}

	// calculer la position des os en edition
	osPosEdition.resize(motion_frame_get_joints_n(frame), NULL);
	rootId = motion_frame_get_root_joint(frame);
	if (rootId==-1)
		throw Erreur("le fichier '"+bvhFileName+"' n'a pas de root !");
	calculOsPosEdition(frame, rootId, Vector3::ZERO);

	// calculer la position de chaque articulation de chaque frame
	osPos.resize(nbrTotalFrames);
	jointsTransf.resize(nbrTotalFrames);
	for (int numFrame=0; numFrame<nbrTotalFrames; numFrame++) {
		osPos[numFrame].resize(motion_frame_get_joints_n(frame), NULL);
		jointsTransf[numFrame].resize(motion_frame_get_joints_n(frame), Transform::IDENTITY);
		// mettre a jour le tableau des os : pour tous les fils du root
		motion_get_frame(frame, motion, numFrame);
		int childId = joint_get_child(frame, rootId);
		while (childId!=-1) {
			// afficher tous les petits fils
			Transform t1, t2;
			calculJoinPos(numFrame, frame, childId, Transform::IDENTITY);
			// passer au fils suivant
			childId = joint_get_next(frame, childId);
		}
	}
	motion_frame_free(frame, true);
	motion_free(motion, true);
}

SkeletonMesh::~SkeletonMesh() {
	for (vector<Os*>::const_iterator it=osPosEdition.begin(); it!=osPosEdition.end(); it++)
		delete *it;
	for (vector<vector<Os*> >::const_iterator itF=osPos.begin(); itF!=osPos.end(); itF++) {
		for (vector<Os*>::const_iterator itO=itF->begin(); itO!=itF->end(); itO++)
			delete *itO;
	}
}

// calculer la position d'edition des os : parcours recursivif du skelete
const Vector3 SkeletonMesh::calculOsPosEdition(MFRAME * frame, int joinId, const Vector3 & accumulateur) {
	Vector3 offset, moyenne = Vector3(0.0f, 0.0f, 0.0f);

	// lire la position de ce joint
	joint_get_offset(frame, joinId, offset);
	offset = orientationEdition * offset * scale + accumulateur;

	// pour tous les fils
	int childId = joint_get_child(frame, joinId);
	int nbrChild = 0;
	while (childId != -1) {
		nbrChild++;
		// calculer la position du fils
		moyenne += calculOsPosEdition(frame, childId, offset);
		// passer au fils suivant
		childId = joint_get_next(frame, childId);
	}

	if (nbrChild >= 1) {
		osPosEdition[joinId] = new Os(offset, moyenne / nbrChild, joinId);
	}

	return offset;
}

// calculer la position de chaque os dans la frame courante : parcours recursivif du skelete
const Vector3 SkeletonMesh::calculJoinPos(int numFrame, MFRAME * frame, int joinId, const Transform & transParent) {
	Vector3 temp, position = Vector3::ZERO, moyenne = Vector3::ZERO;
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
	Transform * local = & jointsTransf[numFrame][joinId];
	*local = Transform(position, orientation);
	Transform global = transParent * (*local);

	// calculer la position de l'articulation dans le repere global
	position = orientationEdition * (global * Vector3::ZERO) * scale;

	// pour tous les fils
	int childId = joint_get_child(frame, joinId);
	int nbrChild = 0;
	while (childId != -1) {
		nbrChild++;
		// calculer la position du fils
		moyenne += calculJoinPos(numFrame, frame, childId, global);
		// passer au fils suivant
		childId = joint_get_next(frame, childId);
	}

	if (nbrChild >= 1) {
		osPos[numFrame][joinId] = new Os(position, moyenne / nbrChild, joinId);
	}

	return position;
}

// acceder a la position d'une articulation dans la frame courante
const SkeletonMesh::Os * SkeletonMesh::getOsPosition(int numFrame, int joinId) const {
	return osPos[numFrame][joinId];
}
const Transform & SkeletonMesh::getJointsTransf(int numFrame, int joinId) const {
	return jointsTransf[numFrame][joinId];
}

// acceder a la position d'un os dans la position d'edition
const SkeletonMesh::Os * SkeletonMesh::getOsPosEdition(int joinId) const {
	return osPosEdition[joinId];
}

// acceder au nom d'une articulation a partir de sont id
const string & SkeletonMesh::getJointName(int joinId) const {
	return jointsNameById[joinId];
}

// acceder a l'identifiant d'une articulation a partir de son nom (attention au doublon seul le permier est enregistre)
int SkeletonMesh::getJointIndex(const string & name) const {
	// chercher ce nom dans la table de hash
	NameIndex::const_iterator itn = jointsName.find(name);
	// si cet objet n'existe pas
	if (itn == jointsName.end())
		throw Erreur("le joint '"+name+"' n'existe pas dans le squelette '"+bvhFileName+"' !");
	// else
	return itn->second;
}

