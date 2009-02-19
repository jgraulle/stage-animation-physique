/*
 * Char2D.h
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef Char2D_H_
#define Char2D_H_

#include "Objet2D.h"

class Char2D: public Objet2D {
public:
	static const int TAILLE_EN_PIXEL = 32;
	static const int NBR_LIGNES = 16;
	static const int NBR_COLONNES = 16;
	static const int LARGEUR_IMAGE = TAILLE_EN_PIXEL*NBR_COLONNES;
	static const int HAUTEUR_IMAGE = TAILLE_EN_PIXEL*NBR_LIGNES;

	// constructeur
	Char2D(char c, const Texture * texture, const Vecteur2 & position = Vecteur2(), const Vecteur2 & taille = Vecteur2());

	// destructeur
	virtual ~Char2D();
};

#endif /* Char2D_H_ */
