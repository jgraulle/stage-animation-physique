/*
 * Capsule.cpp
 *
 *  Created on: 19 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Capsule.h"

#include <GL/gl.h>

Capsule::Capsule(float rayon, float hauteur, int slices, int stacks)
: slices(slices), stacks(stacks), rayon(rayon), hauteur(hauteur) {
	GLUquadric* quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);

	idDisplayList = glGenLists(1);
	glNewList(idDisplayList, GL_COMPILE);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.0, -hauteur/2.0);
	gluCylinder(quadric, rayon, rayon, hauteur, slices, 1);
	glTranslatef(0.0, 0.0, hauteur);
	gluSphere(quadric, rayon, slices, stacks);
	glTranslatef(0.0, 0.0, -hauteur);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	gluSphere(quadric, rayon, slices, stacks);
	glEndList();

	gluDeleteQuadric(quadric);

}

Capsule::~Capsule() {}
