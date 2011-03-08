/*
 * Perso.h
 *
 *  Created on: 26 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef SKELETON_H_
#define SKELETON_H_

#include "SkeletonMesh.h"

#include <MoteurGraphique.h>
#include <bvh.h>

using namespace std;

class Skeleton: public Objet3D {
public:
	// constructeur
	Skeleton(const SkeletonMesh * skeletonMesh, Material material, Transform transform);

	// destructeur
	virtual ~Skeleton();

	// affichage
	virtual void display() const;

	// fonction de mise a jour de l'objet
	virtual void update(f32 elapsed);

	virtual const SkeletonMesh * getSkeletonMesh() const {return skeletonMesh;}
	virtual int getNumFrame() const {return numFrameCourant;}
	virtual void setNumFrame(int numFrame);
	virtual int getNumFrameSuivante() const;
	virtual void setBorne(int numFrameDebut, int numFrameFin);
	virtual void setVitesse(f32 vitesse);

private:
	const SkeletonMesh * skeletonMesh;
	f32 tempsCourant, vitesse;
	int numFrameCourant, numFrameDebut, numFrameFin;
};

#endif /* SKELETON_H_ */
