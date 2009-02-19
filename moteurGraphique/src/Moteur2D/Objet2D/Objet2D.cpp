/*
 * Objet2D.cpp
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Objet2D.h"

#include "../../Ressource/RessourceManager.h"

#include <GL/gl.h>

Objet2D::Objet2D(const Texture * texture, const Vecteur2 & position, const Vecteur2 & taille, f32 angle)
: texture(texture), position(position), taille(taille), angle(angle), estDansMonde(true),
debutCoordTexture(0.0f, 0.0f), finCoordTexture(1.0f, 1.0f) {
	if (taille==Vecteur2() && texture!=NULL)
		this->taille = texture->getTaille();
}

Objet2D::Objet2D(const string & texture, const Vecteur2 & position, const Vecteur2 & taille, f32 angle)
: texture(NULL), position(position), taille(taille), angle(angle), estDansMonde(true),
debutCoordTexture(0.0f, 0.0f), finCoordTexture(1.0f, 1.0f) {
	setTexture(TexturesManager::getInstance()->get(texture));
	if (taille==Vecteur2() && this->texture!=NULL)
		this->taille = this->texture->getTaille();
}


Objet2D::~Objet2D() {}

// deplacer l'objet
void Objet2D::setPosition(const Vecteur2 & position) {
	this->position = position;
}
void Objet2D::setPosition(f32 x, f32 y) {
	position.setX(x);
	position.setY(y);
}
void Objet2D::translate(const Vecteur2 & v) {
	position += v;
}
void Objet2D::translate(f32 x, f32 y) {
	position += Vecteur2(x, y);
}

// tourner un objet
void Objet2D::setRotation(f32 angle) {
	this->angle = angle;
}
void Objet2D::rotate(f32 angle) {
	this->angle += angle;
}

// definir si l'objet est afficher ou pas
void Objet2D::setIsDisplay(bool b) {
	estDansMonde = b;
}


// afficher l'objet
void Objet2D::display() {
	f32 x = position.getX();
	f32 y = position.getY();
	f32 width = taille.getX();
	f32 height = taille.getY();
	f32 xDebutCoordTexture = debutCoordTexture.getX();
	f32 yDebutCoordTexture = debutCoordTexture.getY();
	f32 xFinCoordTexture = finCoordTexture.getX();
	f32 yFinCoordTexture = finCoordTexture.getY();

	if (texture!=NULL) {

		// appliquer la texture
		texture->apply();

		// TODO rotation objet2D

		// dessiner un rectangle
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(xDebutCoordTexture, yDebutCoordTexture);
			glVertex3f(x, y, 0.0f);

			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(xDebutCoordTexture, yFinCoordTexture);
			glVertex3f(x, y+height-1, 0.0f);

			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(xFinCoordTexture, yFinCoordTexture);
			glVertex3f(x+width-1, y+height-1, 0.0f);

			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(xFinCoordTexture, yDebutCoordTexture);
			glVertex3f(x+width-1, y, 0.0f);
		glEnd();
	}
}

// changer la texture
void Objet2D::setTexture(const Texture * texture) {
	this->texture = texture;
}

// changer la taille de l'objet
void Objet2D::setTaille(const Vecteur2 & taille) {
	this->taille = taille;
}

void Objet2D::setTaille(f32 x, f32 y) {
	this->taille = Vecteur2(x, y);
}
