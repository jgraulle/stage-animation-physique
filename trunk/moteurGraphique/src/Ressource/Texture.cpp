/*
 * Texture.cpp
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Texture.h"

#include "../Erreur.h"
#include "RessourceManager.h"

#include <GL/glext.h>

int Texture::modeConvertion(Mode m) {
	switch (m) {
	case UNE_FOIS:
		return GL_CLAMP_TO_EDGE;
	case REPETE:
		return GL_REPEAT;
	case REPETE_MIROIR:
		return GL_MIRRORED_REPEAT_ARB;
	}
	throw Erreur("mode de repetition de la texture inconue");
	return 0;
}

// generer une texture a partir d'une image dans la carte 3D
// l'image peut etre detruit apres generation
Texture::Texture(const string & name, const Image * image, bool mipmap, Mode modeHauteur, Mode modeLargeur)
: name(name), image(image) {
	// generer une texture a partir de l'image
	// creation d'une texture openGL
	glGenTextures(1, &idGL);
	glBindTexture(GL_TEXTURE_2D, idGL);
	// parametre de la texture
	if (mipmap) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modeConvertion(modeHauteur));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modeConvertion(modeLargeur));
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, image->getLargeur(), image->getHauteur(), GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid *)image->getData());
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modeConvertion(modeHauteur));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modeConvertion(modeLargeur));
		glTexImage2D(GL_TEXTURE_2D, 0, 4, image->getLargeur(), image->getHauteur(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid *)image->getData());
	}
}

// generer une texture a partir d'une image dans la carte 3D
// l'image peut etre detruit apres generation
Texture::Texture(const string & imageName, bool mipmap, Mode modeHauteur, Mode modeLargeur) {
	name = imageName;
	image = ImagesManager::getInstance()->get(imageName);
	// generer une texture a partir de l'image
	// creation d'une texture openGL
	glGenTextures(1, &idGL);
	glBindTexture(GL_TEXTURE_2D, idGL);
	// parametre de la texture
	if (mipmap) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modeConvertion(modeHauteur));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modeConvertion(modeLargeur));
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, image->getLargeur(), image->getHauteur(), GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid *)image->getData());
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modeConvertion(modeHauteur));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modeConvertion(modeLargeur));
		glTexImage2D(GL_TEXTURE_2D, 0, 4, image->getLargeur(), image->getHauteur(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid *)image->getData());
	}
}

Texture::~Texture() {
	glDeleteTextures (1, &idGL);
}

void Texture::apply() const {
	glBindTexture(GL_TEXTURE_2D, idGL);
}

// accesseur sur l'image
const Image * Texture::getImage() const {
	return image;
}
Vecteur2 Texture::getTaille() const {
	return Vecteur2(image->getLargeur(), image->getHauteur());
}
