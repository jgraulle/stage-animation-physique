/*
 * Perso.cpp
 *
 *  Created on: 26 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Perso.h"

Perso::Perso(const string & bvhFileName, Material material, Transform transform)
: Objet3D(material, NULL, transform), bvhFileName(bvhFileName) {
	// chargement du fichier bvh
	if(motion_load_bvh(&motion, bvhFileName.c_str()) < 0)
		throw ErreurFileNotFound(bvhFileName, "");
	// creation de la premiere frame
	frame = motion_frame_init(NULL, motion);
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

	motion_get_frame(frame, motion, 0);
	// acces a la position du root
	int rootId = motion_frame_get_root_joint(frame);
	if (rootId==-1)
		throw Erreur("le fichier '"+bvhFileName+"' n'a pas de root !");
	Vector3 offset;
	joint_get_offset(frame, rootId, offset);
	display(rootId, offset);

	glPointSize(1.0f);
}

void Perso::display(int joinId, const Vector3 & accumulateur) const {
	// pour tous les fils
	int childId = joint_get_child(frame, joinId);
	int nbrChild = 0;
	Vector3 offset, moyenne = Vector3(0.0f, 0.0f, 0.0f);
	while (childId!=-1) {
		nbrChild++;
		// lire la position de ce joint
		joint_get_offset(frame, childId, offset);

		// afficher l'os entre le joint pere et le courant
//cout << "id="<< childId << "\tname='" << joint_get_name(frame, childId) << "'\toffset=" << accumulateur+offset << "\tacc="<< accumulateur << endl;
		glBegin(GL_POINTS);
		glColor4f(1.0f, 0.0f, 0.0f, 1.f);
		glVertex3fv(accumulateur);
		glVertex3fv(accumulateur+offset);
		glEnd();

		moyenne+=accumulateur+offset;

		// afficher tous les petits fils
		display(childId, accumulateur+offset);

		// passer au fils suivant
		childId = joint_get_next(frame, childId);
	}

	if (nbrChild>=1) {
		glBegin(GL_LINES);
		glColor4f(1.0f, 1.0f, 1.0f, 1.f);
		glVertex3fv(accumulateur);
		glVertex3fv(moyenne/nbrChild);
		glEnd();
	}
}

// fonction de mise a jour de l'objet
void Perso::update(f32 elapsed) {

}
