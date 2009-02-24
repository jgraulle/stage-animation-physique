/*
 * Objet2D.h
 *
 *  Created on: 6 f�vr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef OBJET2D_H_
#define OBJET2D_H_

#include "../../Math/Vecteur2.h"
#include "../../Type.h"
#include "../../Ressource/Texture.h"

#include <string>

using namespace std;

class Objet2D {
public:
	Objet2D(const Texture * texture, const Vecteur2 & position = Vecteur2(), const Vecteur2 & taille = Vecteur2(), f32 angle = 0.0f);
	Objet2D(const string & texture, const Vecteur2 & position = Vecteur2(), const Vecteur2 & taille = Vecteur2(), f32 angle = 0.0f);
	virtual ~Objet2D();

	// texture
	virtual const Texture * getTexture() const {return texture;}
	virtual void setTexture(const Texture * texture);

	// taille
	virtual const Vecteur2 & getTaille() const {return taille;}
	virtual Vecteur2 & getTaille() {return taille;}
	virtual void setTaille(const Vecteur2 &);
	virtual void setTaille(f32 x, f32 y);

	// position
	virtual const Vecteur2 & getPosition() const {return position;}
	virtual Vecteur2 & getPosition() {return position;}
	virtual void setPosition(const Vecteur2 &);
	virtual void setPosition(f32 x, f32 y);
	virtual void translate(const Vecteur2 &);
	virtual void translate(f32 x, f32 y);

	// inclinaison
	virtual f32 getAngle() const {return angle;}
	virtual void setRotation(f32 angle);
	virtual void rotate(f32 angle);

	// Coordonne de texture
	virtual void setDebutCoordTexture(const Vecteur2 & v) {debutCoordTexture = v;}
	virtual void setFinCoordTexture(const Vecteur2 & v) {finCoordTexture = v;}
	virtual const Vecteur2 & getDebutCoordTexture() const {return debutCoordTexture;}
	virtual const Vecteur2 & getFinCoordTexture() const {return finCoordTexture;}
	virtual Vecteur2 & getDebutCoordTexture() {return debutCoordTexture;}
	virtual Vecteur2 & getFinCoordTexture() {return finCoordTexture;}

	// definir si l'objet est afficher ou pas
	virtual bool isDisplay() const {return estDansMonde;}
	virtual void setIsDisplay(bool);

	// afficher l'objet
	virtual void display();

	// mise a jour de l'objet
	virtual void update(float elapsed) {}

private:
	const Texture * texture;
	Vecteur2 position;
	Vecteur2 taille;
	f32 angle;
	bool estDansMonde;
	Vecteur2 debutCoordTexture;
	Vecteur2 finCoordTexture;
};

#endif /* OBJET2D_H_ */
