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

//#define TRANS_GL

// affichage
void Perso::display() const {
	Disable lumiere(GL_LIGHTING);
	Disable texture(GL_TEXTURE_2D);
	Disable profondeur(GL_DEPTH_TEST);
	glPointSize(3.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.f);
#ifndef TRANS_GL
	glBegin(GL_POINTS);
#endif

	// acces a la position du root
	int rootId = motion_frame_get_root_joint(frame);
	if (rootId==-1)
		throw Erreur("le fichier '"+bvhFileName+"' n'a pas de root !");
	display(rootId, Transform(Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE));

#ifndef TRANS_GL
	glEnd();
#endif
	glPointSize(1.0f);
}

void Perso::display(int joinId, const Transform & transParent) const {
	Vector3 temp, position = Vector3::ZERO;
	Quaternion orientation = Quaternion::IDENTITY;
	int bind[3];
#ifdef TRANS_GL
	glPushMatrix();
#endif

	// acces a la position
	joint_get_offset(frame, joinId, position);
#ifdef TRANS_GL
	glTranslatef(position[0], position[1], position[2]);
#endif
	joint_get_position(frame, joinId, temp);
#ifdef TRANS_GL
	glTranslatef(temp[0], temp[1], temp[2]);
#else
	position += temp;
#endif

	// acces a l'orientation
	joint_get_orientation(frame, joinId, temp, bind);
	for (int i=0; i<3; i++) {
		switch(bind[i]) {
		case JOINT_XROT:
#ifdef TRANS_GL
			glRotatef(temp[i], 1.0f, 0.0f, 0.0f);
#else
			orientation = orientation * Quaternion(temp[i] * M_PI / 180.0f, Vector3::UNIT_X);
#endif
			break;
		case JOINT_YROT:
#ifdef TRANS_GL
			glRotatef(temp[i], 0.0f, 1.0f, 0.0f);
#else
			orientation = orientation * Quaternion(temp[i] * M_PI / 180.0f, Vector3::UNIT_Y);
#endif
			break;
		case JOINT_ZROT:
#ifdef TRANS_GL
			glRotatef(temp[i], 0.0f, 0.0f, 1.0f);
#else
			orientation = orientation * Quaternion(temp[i] * M_PI / 180.0f, Vector3::UNIT_Z);
#endif
			break;
		}
	}

	// application de la transformation
	Transform global;
#ifndef TRANS_GL
	Transform local(position, orientation);
	global = transParent * local;
#endif

#ifdef TRANS_GL
	glBegin(GL_POINTS);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
#else
	Vector3 point = global * Vector3::ZERO;
	glVertex3f(point[0], point[1], point[2]);
#endif

	// pour tous les fils
	int childId = joint_get_child(frame, joinId);
	int nbrChild = 0;
	while (childId!=-1) {
		nbrChild++;

		// afficher tous les petits fils
		display(childId, global);

		// passer au fils suivant
		childId = joint_get_next(frame, childId);
	}
#ifdef TRANS_GL
	glPopMatrix();
#endif
}

// fonction de mise a jour de l'objet
void Perso::update(f32 elapsed) {
	tempsAnim += elapsed;
	if (tempsAnim>motion_frame_get_time(frame)+tempsParFrame) {
		numFrame++;
		if (motion_get_frame(frame, motion, numFrame)!=0) {
			numFrame = 0;
			tempsAnim = 0.0;
			motion_get_frame(frame, motion, numFrame);
		}
	}
}
