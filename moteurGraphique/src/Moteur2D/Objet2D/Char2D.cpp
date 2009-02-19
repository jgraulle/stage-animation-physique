/*
 * Char2D.cpp
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Char2D.h"

Char2D::Char2D(char c, const Texture * texture, const Vecteur2 & position, const Vecteur2 & taille)
: Objet2D(texture, position, taille, 0.0f) {
	f32 xMilieuCoordTexture = (c%NBR_COLONNES)*TAILLE_EN_PIXEL+TAILLE_EN_PIXEL/2;
	f32 yMilieuCoordTexture = c/NBR_COLONNES*TAILLE_EN_PIXEL+TAILLE_EN_PIXEL/2;
	setDebutCoordTexture(Vecteur2((xMilieuCoordTexture-taille.getX()/2.0f)/LARGEUR_IMAGE, (yMilieuCoordTexture-taille.getY()/2.0f)/HAUTEUR_IMAGE));
	setFinCoordTexture(Vecteur2((xMilieuCoordTexture+taille.getX()/2.0f)/LARGEUR_IMAGE, (yMilieuCoordTexture+taille.getY()/2.0f)/HAUTEUR_IMAGE));
}

Char2D::~Char2D() {}
