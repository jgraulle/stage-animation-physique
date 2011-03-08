/*
 * Cone.h
 *
 *  Created on: 5 juin 2009
 *      Author: jeremie
 */

#ifndef CONE_H_
#define CONE_H_

#include "../Mesh.h"

class Cone : public Mesh {
public:
	Cone(int slices);
	virtual ~Cone();
private:
	int slices;
};

#endif /* CONE_H_ */
