/*
 * Moteur.h
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef MOTEUR3D_H_
#define MOTEUR3D_H_

#include "Math/Vecteur2.h"

#include "Moteur2D/Monde2D.h"

#include "Moteur3D/Monde3D.h"

class Moteur {
public:
	// acceder a la seule instance creer du moteur
	static Moteur * getInstance();

	// destructeur
	virtual ~Moteur();

	// taille de l'ecran en pixel
	virtual const Vecteur2 & getTailleEcran() const;
	virtual void setTailleEcran(const Vecteur2 &);

	// met a jour tous les elements
	virtual void update();
	virtual f32 getElapsed();

	// echanger les 2 buffer et afficher le buffer courrant
	virtual void swapBuffer();

	// accesseur
	virtual Monde2D * getMonde2D();
	virtual Monde3D * getMonde3D();

	// affiche le monde 2D et le monde 3D
	virtual void display();

private:
	// constructeur en priver car singleton
	Moteur();

	// couleur de fond
	Couleur background;
	// monde par defaut
	Monde3D * monde3D;
	Monde2D * monde2D;
	// taille de la fenetre en pixel
	Vecteur2 tailleEcran;
	// temps passe entre 2 frames
	f32 elapsed;
};

#endif /* MOTEUR3D_H_ */
