/*
 * Cube.h
 *
 *  Created on: 9 févr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef CUBE_H_
#define CUBE_H_

#include "../Mesh.h"

class Cube: public Mesh {
public:
	Cube(const string & name);
	virtual ~Cube();
};

#endif /* CUBE_H_ */
