/*
 * Couleur.h
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef COULEUR_H_
#define COULEUR_H_

#include "../Type.h"

#include <iostream>

using namespace std;

class Couleur {
public:
	static const int NBR_COULEUR = 4;
	static const Couleur BLANC, NOIR, ROUGE, VERT ,BLEU, GRIS, GRIS_CLAIR, GRIS_FONCE;

	friend ostream & operator << (ostream &, const Couleur &);

	// constructeur
	Couleur(f32 r, f32 v, f32 b, f32 a = 1.0f);
	Couleur(const f32 * couleur);
	Couleur(const u8 * couleur);
	Couleur(const Couleur &);

	// affectation
	const Couleur & operator = (const Couleur &);

	// destructeur
	virtual ~Couleur();

	// accesseur
	f32 getR() const;
	f32 getV() const;
	f32 getB() const;
	f32 getA() const;
	f32 operator [] (int) const;
	const f32 * getData() const;

	// setteur
	void setR(f32);
	void setV(f32);
	void setB(f32);
	void setA(f32);
	void set(f32 r, f32 v, f32 b, f32 a);

private:
	f32 c[NBR_COULEUR];
};

#endif /* COULEUR_H_ */
