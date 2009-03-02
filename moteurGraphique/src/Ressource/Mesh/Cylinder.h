/*
 * Cylinder.h
 *
 *  Created on: 19 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef CYLINDRE_H_
#define CYLINDRE_H_

#include "../Mesh.h"

class Cylinder: public Mesh {
public:
	Cylinder(int slices);
	virtual ~Cylinder();

private:
	int slices;
};

#endif /* CYLINDRE_H_ */
