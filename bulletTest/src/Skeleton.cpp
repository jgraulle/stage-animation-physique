/*
 * Perso.cpp
 *
 *  Created on: 26 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Skeleton.h"

Skeleton::Skeleton(const SkeletonMesh * skeletonMesh, Material material, Transform transform)
: Objet3D(material, NULL, transform), bvhFileName(skeletonMesh), tempsAnim(0.0), numFrame(0) {}

Skeleton::~Skeleton() {
}

// affichage
void Skeleton::display() const {
	Disable lumiere(GL_LIGHTING);
	Disable texture(GL_TEXTURE_2D);
	Disable profondeur(GL_DEPTH_TEST);
	glPointSize(3.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.f);
	glBegin(GL_LINES);

	for (int joinId=0; joinId<bvhFileName->getNbrJoints(); joinId++) {
		if (bvhFileName->getJointPosition(numFrame,joinId)!=NULL) {
			glVertex3fv(bvhFileName->getJointPosition(numFrame,joinId)->debut);
			glVertex3fv(bvhFileName->getJointPosition(numFrame,joinId)->fin);
		}
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.f);
	glPointSize(1.0f);
	glBegin(GL_LINES);
	for (int joinId=0; joinId<bvhFileName->getNbrJoints(); joinId++) {
		if (bvhFileName->getOsPosEdition(joinId)!=NULL) {
			glVertex3fv(bvhFileName->getOsPosEdition(joinId)->debut);
			glVertex3fv(bvhFileName->getOsPosEdition(joinId)->fin);
		}
	}
	glEnd();
}

// fonction de mise a jour de l'objet
void Skeleton::update(f32 elapsed) {
	tempsAnim += elapsed;
	// si le temps est plus grand que le temps de debut de la prochaine frame
	if (tempsAnim>(numFrame+1)*bvhFileName->getTempsParFrame()) {
		// passer a la frame suivante
		numFrame++;
		if (numFrame>=bvhFileName->getNbrTotalFrames()) {
			// repartir a la frame 0 si derniere frame
			numFrame = 0;
			tempsAnim = 0.0;
		}
	}
}
