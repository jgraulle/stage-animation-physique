/*
 * Sphere.h
 *
 *  Created on: 9 f�vr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef SPHERE_H_
#define SPHERE_H_

#include "../Mesh.h"

class Sphere : public Mesh {
public:
	Sphere(int slices, int stacks);
	virtual ~Sphere();
private:
	int slices;	// nombre de tranches
	int stacks;	// nombre de paralleles
};

#endif /* SPHERE_H_ */
