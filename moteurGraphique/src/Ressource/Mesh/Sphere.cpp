/*
 * Sphere.cpp
 *
 *  Created on: 9 f�vr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Sphere.h"

#include <GL/glu.h>

Sphere::Sphere(int slices, int stacks)
: slices(slices), stacks(stacks) {
	GLUquadric * quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);

	idDisplayList = glGenLists(1);
	glNewList(idDisplayList, GL_COMPILE);

	gluSphere(quadric, 1.0, slices, stacks);

	glEndList();

	gluDeleteQuadric(quadric);
}

Sphere::~Sphere() {}
