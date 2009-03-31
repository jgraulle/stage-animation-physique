/*
 * Perso.cpp
 *
 *  Created on: 26 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Perso.h"

Perso::Perso(const string & bvhFileName, Material material, Transform transform)
: Objet3D(material, NULL, transform), bvhFileName(bvhFileName), tempsAnim(0.0), numFrame(0) {
	// chargement du fichier bvh
	if(motion_load_bvh(&motion, bvhFileName.c_str()) < 0)
		throw ErreurFileNotFound(bvhFileName, "");
	// initialisation de la structure frame
	frame = motion_frame_init(NULL, motion);
	// calculer le temps d'une frame
	motion_get_frame(frame, motion, 1);
	tempsParFrame = motion_frame_get_time(frame);
	// retourner a la 1er frame
	motion_get_frame(frame, motion, 0);
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
	glPushMatrix();

	// acces a la position du root
	int rootId = motion_frame_get_root_joint(frame);
	if (rootId==-1)
		throw Erreur("le fichier '"+bvhFileName+"' n'a pas de root !");
	display(rootId);

	glPopMatrix();
	glPointSize(1.0f);
}

void Perso::display(int joinId) const {
	Vector3 offset, position, angleEuler;
	int bind[3];

	glPushMatrix();

	// construire la transformation locale
	joint_get_offset(frame, joinId, offset);
	glTranslatef(offset[0], offset[1], offset[2]);

	joint_get_position(frame, joinId, position);
	glTranslatef(position[0], position[1], position[2]);

	joint_get_orientation(frame, joinId, angleEuler, bind);
	for (int i=0; i<3; i++) {
		switch(bind[i]) {
		case JOINT_XROT:
			glRotatef(angleEuler[i], 1.0f, 0.0f, 0.0f);
			break;
		case JOINT_YROT:
			glRotatef(angleEuler[i], 0.0f, 1.0f, 0.0f);
			break;
		case JOINT_ZROT:
			glRotatef(angleEuler[i], 0.0f, 0.0f, 1.0f);
			break;
		}
	}

	glBegin(GL_POINTS);
	glColor4f(1.0f, 0.0f, 0.0f, 1.f);
	joint_get_position(frame, joinId, position);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();

	// pour tous les fils
	int childId = joint_get_child(frame, joinId);
	int nbrChild = 0;
	while (childId!=-1) {
		nbrChild++;

		// afficher tous les petits fils
		display(childId);

		// passer au fils suivant
		childId = joint_get_next(frame, childId);
	}
	glPopMatrix();
}

// fonction de mise a jour de l'objet
void Perso::update(f32 elapsed) {
	tempsAnim += elapsed;
	if (tempsAnim>motion_frame_get_time(frame)+tempsParFrame) {
		numFrame++;
		cout << numFrame << endl;
		if (motion_get_frame(frame, motion, numFrame)!=0) {
			numFrame = 0;
			tempsAnim = 0.0;
			motion_get_frame(frame, motion, numFrame);
		}
	}
}
