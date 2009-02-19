/*
 * string.h
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef String2D_H_
#define String2D_H_

#include <string>

#include "Objet2D.h"

using namespace std;

class String2D: public Objet2D {
public:
	String2D(const string & texte, const Texture * texture, const Vecteur2 & positionDebut = Vecteur2(), const Vecteur2 & tailleChar2D = Vecteur2());
	virtual ~String2D();

	// afficher l'objet
	virtual void display();

	// accesseur
	const string & getTexte() const {return texte;}
	string & getTexte() {return texte;}

	// modifieur
	void setTexte(const string & s) {texte = s;}

private:
	string texte;
};

#endif /* String2D_H_ */
