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
	Capsule(const string & name, float rayon, float hauteur, int slices, int stacks);
	virtual ~Capsule();

private:
	int slices;
	int stacks;
};

#endif /* CAPSULE_H_ */
