/*
 * Cube.cpp
 *
 *  Created on: 9 f�vr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Cube.h"

#include <GL/gl.h>

Cube::Cube() {
	idDisplayList = glGenLists(1);
	glNewList(idDisplayList, GL_COMPILE);

#define X 1.0f
#define Y 1.0f
#define Z 1.0f

#define P(x, y, z) glVertex3f(x X, y Y, z Z)
#define N(x, y, z) glNormal3f(x, y, z)
#define T(u, v) glTexCoord2f(u, v)

	glBegin(GL_QUADS);			// Draw a Cube

		// dessus
		N(0.0f,1.0f,0.0f); T(0.0f, 0.0f); P(+,+,-);
		N(0.0f,1.0f,0.0f); T(0.0f, 1.0f); P(-,+,-);
		N(0.0f,1.0f,0.0f); T(1.0f, 1.0f); P(-,+,+);
		N(0.0f,1.0f,0.0f); T(1.0f, 0.0f); P(+,+,+);

		// dessous
		N(0.0f,-1.0f,0.0f); T(0.0f, 0.0f); P(+,-,+);
		N(0.0f,-1.0f,0.0f); T(0.0f, 1.0f); P(-,-,+);
		N(0.0f,-1.0f,0.0f); T(1.0f, 1.0f); P(-,-,-);
		N(0.0f,-1.0f,0.0f); T(1.0f, 0.0f); P(+,-,-);

		// devant
		N(0.0f,0.0f,1.0f); T(0.0f, 0.0f); P(+,+,+);
		N(0.0f,0.0f,1.0f); T(0.0f, 1.0f); P(-,+,+);
		N(0.0f,0.0f,1.0f); T(1.0f, 1.0f); P(-,-,+);
		N(0.0f,0.0f,1.0f); T(1.0f, 0.0f); P(+,-,+);

		// derriere
		N(0.0f,0.0f,-1.0f); T(0.0f, 0.0f); P(+,-,-);
		N(0.0f,0.0f,-1.0f); T(0.0f, 1.0f); P(-,-,-);
		N(0.0f,0.0f,-1.0f); T(1.0f, 1.0f); P(-,+,-);
		N(0.0f,0.0f,-1.0f); T(1.0f, 0.0f); P(+,+,-);

		// gauche
		N(-1.0f,0.0f,0.0f); T(0.0f, 0.0f); P(-,+,+);
		N(-1.0f,0.0f,0.0f); T(0.0f, 1.0f); P(-,+,-);
		N(-1.0f,0.0f,0.0f); T(1.0f, 1.0f); P(-,-,-);
		N(-1.0f,0.0f,0.0f); T(1.0f, 0.0f); P(-,-,+);

		// droite
		N(1.0f,0.0f,0.0f); T(0.0f, 0.0f); P(+,+,-);
		N(1.0f,0.0f,0.0f); T(0.0f, 1.0f); P(+,+,+);
		N(1.0f,0.0f,0.0f); T(1.0f, 1.0f); P(+,-,+);
		N(1.0f,0.0f,0.0f); T(1.0f, 0.0f); P(+,-,-);

	glEnd();								// Done Drawing The Quad

#undef T
#undef N
#undef P

#undef X
#undef Y
#undef Z

	glEndList();
}

Cube::~Cube() {}
