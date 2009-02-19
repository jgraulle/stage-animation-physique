/*
 * Mesh.cpp
 *
 *  Created on: 9 févr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Mesh.h"

Mesh::Mesh(const string & name, f32 positions[][3], f32 normals[][3], f32 texCoords[][2], Face faces[], int nbrFace)
: name(name) {
	idDisplayList = glGenLists(1);
	glNewList(idDisplayList, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for (int f=0; f<nbrFace; f++) {
		for (u16 numVertex = 0; numVertex<3; numVertex++) {
			u16 indexVertex = faces[f].positionIndex[numVertex];
			glVertex3f(positions[indexVertex][0],
			           positions[indexVertex][1],
					   positions[indexVertex][2]);
			indexVertex = faces[f].normalIndex[numVertex];
			glNormal3f(normals[indexVertex][0],
					   normals[indexVertex][1],
					   normals[indexVertex][2]);
			indexVertex = faces[f].texCoordIndex[numVertex];
			glTexCoord2f(texCoords[indexVertex][0],
						 texCoords[indexVertex][1]);
		}
	}
	glEnd();
	glEndList();
}

Mesh::Mesh(const string & name, f32 positions[][3], f32 normals[][3], f32 texCoords[][2], u16 faces[][3][3], int nbrFace)
: name(name) {
	idDisplayList = glGenLists(1);
	glNewList(idDisplayList, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for (int f=0; f<nbrFace; f++) {
		for (u16 numVertex = 0; numVertex<3; numVertex++) {
			u16 indexVertex = faces[f][0][numVertex];
			glVertex3f(positions[indexVertex][0],
			           positions[indexVertex][1],
					   positions[indexVertex][2]);
			indexVertex = faces[f][1][numVertex];
			glNormal3f(normals[indexVertex][0],
					   normals[indexVertex][1],
					   normals[indexVertex][2]);
			indexVertex = faces[f][2][numVertex];
			glTexCoord2f(texCoords[indexVertex][0],
						 texCoords[indexVertex][1]);
		}
	}
	glEnd();
	glEndList();
}

Mesh::Mesh(const string & name, vector<Vector3> positions, vector<Vector3> normals, vector<Vecteur2> texCoords, vector<Face> faces)
: name(name) {
	idDisplayList = glGenLists(1);
	glNewList(idDisplayList, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for (vector<Face>::iterator it = faces.begin();
	     it != faces.end();
	     it++) {
		for (u16 numVertex = 0; numVertex<3; numVertex++) {
			u16 indexVertex = (*it).positionIndex[numVertex];
			assert(indexVertex<positions.size());
			glVertex3f(positions[indexVertex].x,
			           positions[indexVertex].y,
					   positions[indexVertex].z);
			indexVertex = (*it).normalIndex[numVertex];
			assert(indexVertex<normals.size());
			glNormal3f(normals[indexVertex].x,
					   normals[indexVertex].y,
					   normals[indexVertex].z);
			indexVertex = (*it).texCoordIndex[numVertex];
			assert(indexVertex<texCoords.size());
			glTexCoord2f(texCoords[indexVertex].getX(),
						 texCoords[indexVertex].getY());
		}
	}
	glEnd();
	glEndList();
}

Mesh::Mesh(const string & name)
: name(name) {}

Mesh::~Mesh() {
	glDeleteLists(idDisplayList, 1);
}

void Mesh::display() const {
	glCallList(idDisplayList);
}
