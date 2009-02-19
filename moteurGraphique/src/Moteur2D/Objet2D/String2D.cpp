/*
 * string.cpp
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#include "String2D.h"

#include "Char2D.h"
#include "../../Moteur.h"
#include "../../Ressource/Texture.h"

String2D::String2D(const string & texte, const Texture * texture, const Vecteur2 & positionDebut, const Vecteur2 & tailleChar2D)
: Objet2D(texture, positionDebut, tailleChar2D, 0.0f), texte(texte) {
}

String2D::~String2D() {
}

// afficher l'objet
void String2D::display() {
	Vecteur2 pos = getPosition();
	for (u32 i=0; i<texte.length(); i++) {
		switch (texte[i]) {
		case '\n' :
			pos.setX(0.0f);
			pos.addY(getTaille().getY());
			break;
		case ' ' :
			pos.addX(getTaille().getX());
			break;
		default:
			Char2D c(texte[i], getTexture(), pos, getTaille());
			c.display();
			pos.setX(pos.getX()+getTaille().getX());
			if (pos.getX()+getTaille().getX()>Moteur::getInstance()->getTailleEcran().getX()) {
				pos.setX(0.0f);
				pos.addY(getTaille().getY());
			}
		}
	}
}

