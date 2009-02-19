/*
 * Texture.h
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Image.h"
#include "../Math/Vecteur2.h"

#include <string>
#include <GL/gl.h>

using namespace std;

class Texture {
public:
	enum Mode {UNE_FOIS, REPETE, REPETE_MIROIR};

	// generer une texture a partir d'une image dans la carte 3D
	// l'image peut etre detruit apres generation
	Texture(const string & name, const Image * image, bool mipmap = false, Mode modeHauteur = UNE_FOIS, Mode modeLargeur = UNE_FOIS);

	// generer une texture a partir d'une image dans la carte 3D
	// l'image peut etre detruit apres generation
	Texture(const string & imageName, bool mipmap = false, Mode modeHauteur = UNE_FOIS, Mode modeLargeur = UNE_FOIS);

	// destruction de la texture dans la carte 3D
	virtual ~Texture();

	// moteur : appliquer la texture pour les prochains vertex
	virtual void apply() const;

	// accesseur sur l'image
	const Image * getImage() const;
	Vecteur2 getTaille() const;

	virtual const string & getName() const {return name;}
private:
	string name;
	const Image * image;
	int modeConvertion(Mode m);
	GLuint idGL;
};

#endif /* TEXTURE_H_ */
