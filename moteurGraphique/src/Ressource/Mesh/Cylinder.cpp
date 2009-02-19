/*
 * Cylinder.cpp
 *
 *  Created on: 19 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Cylinder.h"

#include <GL/gl.h>

Cylinder::Cylinder(const string & name, int slices)
: Mesh(name), slices(slices) {
	GLUquadric* quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);

	idDisplayList = glGenLists(1);
	glNewList(idDisplayList, GL_COMPILE);
//	glRotatef(90.0, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.0, -1.0);
	gluCylinder(quadric, 1.0, 1.0, 2.0, slices, 1);
	glTranslatef(0.0, 0.0, 2.0);
	gluDisk(quadric, 0.0, 1.0, slices, 1);
	glTranslatef(0.0, 0.0, -2.0);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	gluDisk(quadric, 0.0, 1.0, slices, 1);
	glEndList();

	gluDeleteQuadric(quadric);

}

Cylinder::~Cylinder() {}
