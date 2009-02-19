/*
 * Mesh.h
 *
 *  Created on: 9 févr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef MESH_H_
#define MESH_H_

#include "../Math/Vector3.h"
#include "../Math/Vecteur2.h"

#include <string>
#include <vector>
#include <GL/glu.h>

using namespace std;

class Mesh {
public:
	struct Face {
		u16 positionIndex[3];
		u16 normalIndex[3];
		u16 texCoordIndex[3];
	};

	// construction d'une mesh quelconque, tout les tableaux peuvent etre libere apres utilisation
	Mesh(const string & name, f32 positions[][3], f32 normals[][3], f32 texCoords[][2], Face faces[], int nbrFace);
	Mesh(const string & name, f32 positions[][3], f32 normals[][3], f32 texCoords[][2], u16 faces[][3][3], int nbrFace);
	Mesh(const string & name, vector<Vector3> positions, vector<Vector3> normals, vector<Vecteur2> texCoords, vector<Face> faces);
	virtual ~Mesh();
	virtual void display() const;
	virtual const string & getName() const {return name;}
protected:
	Mesh(const string &);
	string name;
	GLuint idDisplayList;
};

#endif /* MESH_H_ */
