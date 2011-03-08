/*
 * Perso.cpp
 *
 *  Created on: 26 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Perso.h"

Perso::Perso(const string & bvhFileName, Material material, Transform transform)
: Objet3D(material, NULL, transform), bvhFileName(bvhFileName), numFrame(0), tempsCourant(0.0f), vitesse(1.0f)
, transformationRoot(true), translationChildren(true) {
	// chargement du fichier bvh
	if(motion_load_bvh(&motion, bvhFileName.c_str()) < 0)
		throw ErreurFileNotFound(bvhFileName, "");
	// initialisation de la structure frame
	frame = motion_frame_init(NULL, motion);
	// calculer le temps d'une frame
	motion_get_frame(frame, motion, 1);
	tempsParFrame = motion_frame_get_time(frame);
	motion_get_frame(frame, motion, numFrame);
	// compter le nombre total de frame
	numFrame = 0;
	while (motion_get_frame(frame, motion, numFrame)==0)
		numFrame++;
	nbrFrame = numFrame;
	numFrame = 0;
}

Perso::~Perso() {
	motion_frame_free(frame, true);
	motion_free(motion, true);
}

// affichage
void Perso::display() const {
	Disable lumiere(GL_LIGHTING);
	Disable texture(GL_TEXTURE_2D);
	Disable profondeur(GL_DEPTH_TEST);
	glPointSize(3.0f);

	// acces au root
	int rootId = motion_frame_get_root_joint(frame);
	if (rootId==-1)
		throw Erreur("le fichier '"+bvhFileName+"' n'a pas de root !");

	Transform rootTrans = Transform::IDENTITY;
	// si on applique les transformations du root
	if (transformationRoot) {
		// position du root
		Vector3 position;
		joint_get_position(frame, rootId, position);
		if (position==Vector3::ZERO)
			joint_get_offset(frame, rootId, position);
		rootTrans.setPosition(position);
		// orientation du root
		int bind[3];
		Vector3 temp;
		joint_get_orientation(frame, rootId, temp, bind);
		Quaternion orientation = bvhToQuater(temp, bind);
		rootTrans.setRotation(orientation);
	}
	// afficher tout les enfants
	int childId = joint_get_child(frame, rootId);
	while (childId!=-1) {
		// calcul de la position de chaque os de la frame courante
		display(childId, rootTrans);
		// passer au fils suivant
		childId = joint_get_next(frame, childId);
	}

	glPointSize(1.0f);
}

const Vector3 Perso::display(int joinId, const Transform & transParent) const {
	Vector3 position;

	if (translationChildren) {
		joint_get_position(frame, joinId, position);
		if (position==Vector3::ZERO)
			joint_get_offset(frame, joinId, position);
	} else
		joint_get_offset(frame, joinId, position);



	// acces a l'orientation
	int bind[3];
	Vector3 temp;
	joint_get_orientation(frame, joinId, temp, bind);
	Quaternion orientation = bvhToQuater(temp, bind);

	// application de la transformation
	Transform local = Transform(position, orientation);
	Transform global = transParent * local;

	// calculer la position de l'articulation dans le repere global
	position = global * Vector3::ZERO;

	// pour tous les fils
	Vector3 moyenne = Vector3::ZERO;
	int childId = joint_get_child(frame, joinId);
	int nbrChild = 0;
	while (childId != -1) {
		nbrChild++;
		// calculer la position du fils
		moyenne += display(childId, global);
		// passer au fils suivant
		childId = joint_get_next(frame, childId);
	}

	glBegin(GL_POINTS);
	glColor4f(1.0f, 0.0f, 0.0f, 1.f);
	glVertex3fv(position);
	glEnd();

	if (nbrChild>0) {
		glBegin(GL_LINES);
		glColor4f(1.0f, 1.0f, 1.0f, 1.f);
		glVertex3fv(position);
		glVertex3fv(moyenne / nbrChild);
		glEnd();
	}

	return position;
}

// fonction de mise a jour de l'objet
void Perso::update(f32 elapsed) {
	tempsCourant += elapsed*vitesse;
	// si le temps est plus grand que le temps de debut de la prochaine frame
	if (tempsCourant>numFrame*tempsParFrame) {
		// passer a la frame suivante
		numFrame++;
		if (motion_get_frame(frame, motion, numFrame)!=0) {
			tempsCourant = 0.0f;
			numFrame = 0;
			motion_get_frame(frame, motion, numFrame);
		}
	}
}

// passer a la frame suivante manuellement
void Perso::nextFrame() {
	tempsCourant = numFrame*tempsParFrame;
	numFrame++;
	if (motion_get_frame(frame, motion, numFrame)!=0) {
		tempsCourant = 0.0f;
		numFrame = 0;
		motion_get_frame(frame, motion, numFrame);
	}
	cout << numFrame << "/" << nbrFrame << endl;
}
void Perso::previousFrame() {
	numFrame--;
	tempsCourant = numFrame*tempsParFrame;
	if (numFrame==-1) {
		numFrame = nbrFrame-1;
	}
	motion_get_frame(frame, motion, numFrame);
	cout << numFrame << "/" << nbrFrame << endl;
}

// convertion d'angle d'euler de la bvh vers quaternion
Quaternion Perso::bvhToQuater(f32 r[3], int bindings[3]) {
	Quaternion orientation = Quaternion::IDENTITY;
	for (int i=0; i<3; i++) {
		switch(bindings[i]) {
		case JOINT_XROT:
			orientation = orientation * Quaternion(r[i] * M_PI / 180.0f, Vector3::UNIT_X);
			break;
		case JOINT_YROT:
			orientation = orientation * Quaternion(r[i] * M_PI / 180.0f, Vector3::UNIT_Y);
			break;
		case JOINT_ZROT:
			orientation = orientation * Quaternion(r[i] * M_PI / 180.0f, Vector3::UNIT_Z);
			break;
		}
	}
	return orientation;
}

