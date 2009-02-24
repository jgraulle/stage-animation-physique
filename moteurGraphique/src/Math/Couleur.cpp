/*
 * Couleur.cpp
 *
 *  Created on: 6 f�vr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Couleur.h"

#include <assert.h>

const Couleur Couleur::BLANC = Couleur(1.0f, 1.0f, 1.0f, 1.0f);
const Couleur Couleur::NOIR = Couleur(0.0f, 0.0f, 0.0f, 1.0f);
const Couleur Couleur::ROUGE = Couleur(1.0f, 0.0f, 0.0f, 1.0f);
const Couleur Couleur::VERT = Couleur(0.0f, 1.0f, 0.0f, 1.0f);
const Couleur Couleur::BLEU = Couleur(0.0f, 0.0f, 1.0f, 1.0f);
const Couleur Couleur::GRIS = Couleur(0.5f, 0.5f, 0.5f, 1.0f);
const Couleur Couleur::GRIS_CLAIR = Couleur(0.8f, 0.8f, 0.8f, 1.0f);
const Couleur Couleur::GRIS_FONCE = Couleur(0.2f, 0.2f, 0.2f, 1.0f);

std::ostream & operator << (std::ostream & os, const Couleur & c) {
	os << '(' << c.c[0] << ',' << c.c[1] << ',' << c.c[2] << ',' << c.c[3] << ')';
	return os;
}

Couleur::Couleur(f32 r, f32 v, f32 b, f32 a) {
	c[0] = r;
	c[1] = v;
	c[2] = b;
	c[3] = a;
}

Couleur::Couleur(const Couleur & cou) {
	for (int i=0; i<NBR_COULEUR; i++)
		c[i] = cou[i];
}

Couleur::Couleur(const f32 * couleur) {
	assert(couleur!=NULL);
	for (int i=0; i<NBR_COULEUR; i++)
		c[i] = *(couleur+i);
}

Couleur::Couleur(const u8 * couleur) {
	assert(couleur!=NULL);
	for (int i=0; i<NBR_COULEUR; i++)
		c[i] = (*(couleur+i))/255.0f;
}

Couleur::~Couleur() {}

// affectation
const Couleur & Couleur::operator = (const Couleur & cou) {
	for (int i=0; i<NBR_COULEUR; i++)
		c[i] = cou[i];
}

// accesseur
f32 Couleur::getR() const {
	return c[0];
}

f32 Couleur::getV() const {
	return c[1];
}

f32 Couleur::getB() const {
	return c[2];
}

f32 Couleur::getA() const {
	return c[3];
}

f32 Couleur::operator [] (int i) const {
	return c[i];
}

const f32 * Couleur::getData() const {
	return c;
}

// setteur
void Couleur::setR(f32 r) {
	c[0] = r;
}

void Couleur::setV(f32 v) {
	c[1] = v;
}

void Couleur::setB(f32 b) {
	c[2] = b;
}

void Couleur::setA(f32 a) {
	c[3] = a;
}
void Couleur::set(f32 r, f32 v, f32 b, f32 a) {
	c[0] = r;
	c[1] = v;
	c[2] = b;
	c[3] = a;
}
