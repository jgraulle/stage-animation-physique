/*
 * Capsule.h
 *
 *  Created on: 19 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef CAPSULE_H_
#define CAPSULE_H_

#include "../Mesh.h"

class Capsule: public Mesh {
public:
	Capsule(float rayon, float hauteur, int slices, int stacks);
	virtual ~Capsule();

	float getRayon() {return rayon;}
	float gethauteur() {return hauteur;}

private:
	int slices;
	int stacks;
	float rayon, hauteur;
};

#endif /* CAPSULE_H_ */
