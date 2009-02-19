/*
 * Image.h
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include "../Erreur.h"
#include "../Math/Couleur.h"

#include <string>

using namespace std;

class Image {
public:
	// constructeur
	Image(const string & fileName);
	// leve ErreurFileNotFound si l'image n'a pas ete trouve

	// constructeur
	virtual ~Image();

	// accesseur
	virtual int getHauteur() const;
	virtual int getLargeur() const;
	virtual const string & getName() const {return name;}
	// retourne un pointeur sur un tableau de pixel au format RGBA8
	virtual const u8 * getData() const;
	virtual Couleur getCouleur(int h, int l) const;
	virtual const u8 * getData(int h, int l) const;
	virtual u8 getData(int h, int l, u8 c) const;

	// erreur convertion de l'image
	class ErreurDevil : public Erreur
	{ public:	ErreurDevil(string description) : Erreur("erreur dans la librairie devil : "+description) {} };

protected:
	int hauteur;			// en pixel
	int largeur;			// en pixel
	string name;			// nom du fichier de l'image
	void * data;				// tableau des pixel au format RGBA8
};

#endif /* IMAGE_H_ */
