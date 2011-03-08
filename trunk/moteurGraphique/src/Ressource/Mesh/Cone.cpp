/*
 * Cone.cpp
 *
 *  Created on: 5 juin 2009
 *      Author: jeremie
 */

#include "Cone.h"

Cone::Cone(int slices)
: slices(slices) {
	GLUquadric* quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);

	idDisplayList = glGenLists(1);
	glNewList(idDisplayList, GL_COMPILE);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.0, -0.5);
	gluCylinder(quadric, 0.0, 1.0, 1.0, slices, slices);
	glTranslatef(0.0, 0.0, 1.0);
	gluDisk(quadric, 0.0, 1.0, slices, slices);
	glEndList();

	gluDeleteQuadric(quadric);

}

Cone::~Cone() {
}
