/*
 * Image.cpp
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Image.h"

#include "../Type.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// constructeur
Image::Image(const string & fileName)
: name(fileName) {
	ILuint handle;
	// fixe le haut et bas de l'image
	ilEnable(IL_ORIGIN_SET);
	// creation et ouverture d'une image
	ilGenImages(1, &handle);
	ilBindImage(handle);
	char tampon[80];
	strcpy(tampon, fileName.c_str());
	if (!ilLoadImage(tampon))
		throw ErreurFileNotFound(fileName, iluErrorString(ilGetError()));
	// lire la dimension de l'image
	largeur = ilGetInteger(IL_IMAGE_WIDTH);
	hauteur = ilGetInteger(IL_IMAGE_HEIGHT);
	// allocation de la memoire pour l'image (4 composante RGBA)
	data = malloc(largeur * hauteur * 4 * sizeof(unsigned char));
	// recopier les pixel
	ilCopyPixels(0, 0, 0, largeur, hauteur, 1, IL_RGBA, IL_UNSIGNED_BYTE, data);
	// supprimer l'image
	ilDeleteImages(1, &handle);
	ILenum error = ilGetError();
	if (error != IL_NO_ERROR)
	   throw ErreurDevil(iluErrorString(error));
}

Image::~Image() {
	free(data);
}

// accesseur
int Image::getHauteur() const {
	return hauteur;
}
int Image::getLargeur() const {
	return largeur;
}

const u8 * Image::getData() const {
	return (const u8 *)data;
}
Couleur Image::getCouleur(int h, int l) const {
	assert(h<hauteur);
	assert(l<largeur);
	return Couleur(((const u8 *)data)+h*largeur*4+l*4);
}
const u8 * Image::getData(int h, int l) const {
	assert(h<hauteur);
	assert(l<largeur);
	return ((const u8 *)data)+h*largeur*4+l*4;
}
u8 Image::getData(int h, int l, u8 c) const {
	assert(c<4);
	assert(h<hauteur);
	assert(l<largeur);
	return *(((const u8 *)data)+h*largeur*4+l*4+c);
}

