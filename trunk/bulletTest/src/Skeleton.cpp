/*
 * Perso.cpp
 *
 *  Created on: 26 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Skeleton.h"

Skeleton::Skeleton(const SkeletonMesh * skeletonMesh, Material material, Transform transform)
: Objet3D(material, NULL, transform), skeletonMesh(skeletonMesh),
  tempsCourant(0.0), vitesse(1.0f), numFrameCourant(0), numFrameDebut(0) {
	numFrameFin = skeletonMesh->getNbrTotalFrames()-1;
}

Skeleton::~Skeleton() {
}

// affichage
void Skeleton::display() const {
	Disable lumiere(GL_LIGHTING);
	Disable texture(GL_TEXTURE_2D);
	Disable profondeur(GL_DEPTH_TEST);
	glBegin(GL_LINES);
/*
	glColor4f(1.0f, 1.0f, 1.0f, 1.f);
	for (int joinId=0; joinId<skeletonMesh->getNbrJoints(); joinId++) {
		if (skeletonMesh->isOsPosition(numFrameCourant,joinId)) {
			glVertex3fv(skeletonMesh->getOsPosition(numFrameCourant,joinId, SkeletonMesh::DEBUT));
			glVertex3fv(skeletonMesh->getOsPosition(numFrameCourant,joinId, SkeletonMesh::FIN));
		}
	}
*/
/*
	for (int joinId=0; joinId<skeletonMesh->getNbrJoints(); joinId++) {
		if (skeletonMesh->isOsPosEdition(joinId)) {
			if (skeletonMesh->getJointName(joinId)=="LeftHip" || skeletonMesh->getJointName(joinId)=="LeftUpLeg")
				glColor4f(0.0f, 1.0f, 0.0f, 1.f);
			else
				glColor4f(0.0f, 0.3f, 0.0f, 1.f);
			glVertex3fv(skeletonMesh->getOsPosEdition(joinId)->debut);
			glColor4f(0.0f, 0.0f, 0.0f, 1.f);
			glVertex3fv(skeletonMesh->getOsPosEdition(joinId)->fin);
		}
	}
	glColor4f(1.0f, 1.0f, 1.0f, 1.f);
*/
	glEnd();
}

// fonction de mise a jour de l'objet
void Skeleton::update(f32 elapsed) {
	tempsCourant += elapsed*vitesse;
	// si le temps est plus grand que le temps de debut de la prochaine frame
	if (tempsCourant>(numFrameCourant-numFrameDebut+1)*skeletonMesh->getTempsParFrame()) {
		// passer a la frame suivante
		numFrameCourant++;
		if (numFrameCourant>=numFrameFin) {
			// repartir a la frame 0 si derniere frame
			numFrameCourant = numFrameDebut;
			tempsCourant = 0.0;
		}
	}
}

int Skeleton::getNumFrameSuivante() const {
	if (numFrameCourant+1>=numFrameFin) {
		return numFrameDebut;
	}
	return numFrameCourant+1;
}

void Skeleton::setNumFrame(int numFrame) {
	if (numFrameDebut<=numFrame && numFrame<=numFrameFin) {
		numFrameCourant = numFrame;
		tempsCourant = (numFrameCourant-numFrameDebut)*skeletonMesh->getTempsParFrame();
	}
}

void Skeleton::setBorne(int numFrameDebut, int numFrameFin) {
	this->numFrameDebut = numFrameDebut;
	this->numFrameFin = numFrameFin;
	if (numFrameCourant<numFrameDebut || numFrameFin<numFrameCourant) {
		numFrameCourant = numFrameDebut;
		tempsCourant = 0.0;
	}
}

void Skeleton::setVitesse(f32 vitesse) {
	this->vitesse = vitesse;
}
