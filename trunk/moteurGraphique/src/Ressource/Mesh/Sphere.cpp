/*
 * Sphere.cpp
 *
 *  Created on: 9 févr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Sphere.h"

#include <GL/glu.h>

Sphere::Sphere(const string & name, int slices, int stacks)
: Mesh(name), slices(slices), stacks(stacks) {
	GLUquadric * quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);

	idDisplayList = glGenLists(1);
	glNewList(idDisplayList, GL_COMPILE);

	gluSphere(quadric, 1.0, slices, stacks);

	glEndList();

	gluDeleteQuadric(quadric);
}

Sphere::~Sphere() {}
