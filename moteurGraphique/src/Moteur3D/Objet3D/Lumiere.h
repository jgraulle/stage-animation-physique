/*
 * Lumiere.h
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef LUMIERE_H_
#define LUMIERE_H_

#include "../../Math/Vector3.h"
#include "../../Math/Couleur.h"
#include "../../Math/Quaternion.h"

#include <string>

using namespace std;

class Lumiere {
public:
	static const Couleur AMBIENT_DEFAUT;
	static const Couleur DIFFUSE_SPECULAR_DEFAUT;

	Lumiere(const Vector3 & position = Vector3(), const Couleur & c = DIFFUSE_SPECULAR_DEFAUT, f32 attnConstant = 1.0f, f32 attnLinear = 0.0f, f32 attnQuadratic = 0.0f, u8 exposant = 0, f32 angle = 180.0f, const Vector3 & direction = Vector3(0.0f, 0.0f, -1.0f));
	Lumiere(const Vector3 & position = Vector3(), const Couleur & ambientColor, const Couleur & diffuseColor,  const Couleur & specularColor, f32 attnConstant = 1.0f, f32 attnLinear = 0.0f, f32 attnQuadratic = 0.0f, u8 exposant = 0, f32 angle = 180.0f, const Vector3 & direction = Vector3(0.0f, 0.0f, -1.0f));
	Lumiere(const Vector3 & position, const Quaternion & rotation);
	virtual ~Lumiere();

	virtual const Vector3 & getPosition() const;
	virtual void setPosition(const Vector3 &);

	// couleur
	virtual const Couleur & getAmbient() const {return ambientColor;}
	virtual Couleur & getAmbient() {return ambientColor;}
	virtual const Couleur & getDiffuse() const {return diffuseColor;}
	virtual Couleur & getDiffuse() {return diffuseColor;}
	virtual const Couleur & getSpecular() const {return specularColor;}
	virtual Couleur & getSpecular() {return specularColor;}
	void setAmbient(const Couleur & c) {ambientColor = c;}
	void setDiffuse(const Couleur & c) {diffuseColor = c;}
	void setSpecular(const Couleur & c) {specularColor = c;}

	// spot
	virtual const Vector3 & getDirection() const;	// vecteur normalise
	virtual void setDirection(const Vector3 &);		// attention le vecteur doit etre normalise
	virtual void setDirection(const Quaternion &);	// attention le quaternion doit etre normalise
	virtual f32 getAngle() const;					// angle en degres
	virtual void setAngle(f32);						// angle en degres
	virtual u8 getExposant() const;					// attenuation par rapport a l'angle (entre 0 et 128)
	virtual void setExposant(u8);					// attenuation par rapport a l'angle (entre 0 et 128)

	// attenuation
	virtual f32 getAttnConstant() const;
	virtual void setAttnConstant(f32);
	virtual f32 getAttnLinear() const;
	virtual void setAttnLinear(f32);
	virtual f32 getAttnQuadratic() const;
	virtual void setAttnQuadratic(f32);
	virtual void setAttn(f32 constant, f32 linear, f32 quadratic);

	// moteur : afficher la lumiere
	virtual void display(u8 numLight);

private:
	Vector3 position;
	Couleur ambientColor;
	Couleur diffuseColor;
	Couleur specularColor;
	Vector3 direction;
	f32 angle;
	u8 exposant;
	f32 attnConstant;
	f32 attnLinear;
	f32 attnQuadratic;
};

#endif /* LUMIERE_H_ */
