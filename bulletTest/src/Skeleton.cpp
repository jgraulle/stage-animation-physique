/*
 * Perso.cpp
 *
 *  Created on: 26 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Skeleton.h"

Skeleton::Skeleton(const SkeletonMesh * skeletonMesh, Material material, Transform transform)
: Objet3D(material, NULL, transform), skeletonMesh(skeletonMesh), tempsAnim(0.0), numFrame(0) {}

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

	for (int joinId=0; joinId<skeletonMesh->getNbrJoints(); joinId++) {
		if (skeletonMesh->isOsPosition(numFrame,joinId)) {
			glVertex3fv(skeletonMesh->getOsPosition(numFrame,joinId)->debut);
			glVertex3fv(skeletonMesh->getOsPosition(numFrame,joinId)->fin);
		}
	}

/*	glColor4f(1.0f, 1.0f, 1.0f, 1.f);
	for (int joinId=0; joinId<skeletonMesh->getNbrJoints(); joinId++) {
		if (skeletonMesh->isOsPosEdition(joinId)) {
			glVertex3fv(skeletonMesh->getOsPosEdition(joinId)->debut);
			glVertex3fv(skeletonMesh->getOsPosEdition(joinId)->fin);
		}
	}
*/
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.f);
	glPointSize(1.0f);
}

// fonction de mise a jour de l'objet
void Skeleton::update(f32 elapsed) {
	tempsAnim += elapsed/4.0f;
	// si le temps est plus grand que le temps de debut de la prochaine frame
	if (tempsAnim>(numFrame+1)*skeletonMesh->getTempsParFrame()) {
		// passer a la frame suivante
		numFrame++;
		if (numFrame>=skeletonMesh->getNbrTotalFrames()) {
			// repartir a la frame 0 si derniere frame
			numFrame = 0;
			tempsAnim = 0.0;
		}
	}
	// TODO debug
//	numFrame=119;
}
