/*
 * Material.h
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include "../Math/Couleur.h"
#include "../Ressource/Texture.h"

class Material {
public:
	static const Couleur AMBIENT_DEFAUT;
	static const Couleur DIFFUSE_DEFAUT;
	static const Couleur SPECULAR_DEFAUT;
	static const Couleur EMISSION_DEFAUT;
	static const u8 SHININESS_DEFAUT;

	// la texture ne doit pas etre detruite tant que le materiel est utiliser par un objet
	// par contre la dessalocation de la texture doit etre fait par l'utilisateur
	Material(const Texture * texture = NULL, const Couleur & ambient = AMBIENT_DEFAUT, const Couleur & diffuse = DIFFUSE_DEFAUT, const Couleur & specularColor = SPECULAR_DEFAUT, const Couleur & emissionColor = EMISSION_DEFAUT, u8 shininess = SHININESS_DEFAUT);

	// la texture doit etre charger dans le gestionnaire de ressource
	Material(const string & texture, const Couleur & ambient = AMBIENT_DEFAUT, const Couleur & diffuse = DIFFUSE_DEFAUT, const Couleur & specularColor = SPECULAR_DEFAUT, const Couleur & emissionColor = EMISSION_DEFAUT, u8 shininess = SHININESS_DEFAUT);

	// constructeur de copie
	Material(const Material &);

	// la destruction du material ne detruit pas la texture
	virtual ~Material();

	// moteur : appliquer le material pour les prochains vertex
	void apply() const;

	// accesseur
	const Texture * getTexture() const {return texture;}
	const Couleur & getAmbient() const {return ambientColor;}
	Couleur & getAmbient() {return ambientColor;}
	const Couleur & getDiffuse() const {return diffuseColor;}
	Couleur & getDiffuse() {return diffuseColor;}
	const Couleur & getSpecular() const {return specularColor;}
	Couleur & getSpecular() {return specularColor;}
	const Couleur & getEmission() const {return emissionColor;}
	Couleur & getEmission() {return emissionColor;}
	u8 getShininess() const {return shininess;}

	// modifieur
	void setTexture(const Texture * t) {texture = t;}
	void setAmbient(const Couleur & c) {ambientColor = c;}
	void setDiffuse(const Couleur & c) {diffuseColor = c;}
	void setSpecular(const Couleur & c) {specularColor = c;}
	void setEmission(const Couleur & c) {emissionColor = c;}
	void setShininess(u8 s) {assert(s<=128); shininess = s;}

private :
	Couleur ambientColor;		// couleur ambiante de ce material ; par defaut : gris fonce (0.2,0.2,0.2,1.0)
	Couleur diffuseColor;		// couleur diffusee par ce material ; par defaut gris clair (0.8,0.8,0.8,1.0)
	Couleur specularColor;		// couleur de la tache speculaire par ce material ; par defaut gris clair (0.8,0.8,0.8,1.0)
	Couleur emissionColor;		// couleur emis par ce material ; par defaut noir (0.0,0.0,0.0,1.0)
	int shininess;				// coefficient de brillance pour ce material entre 0 et 128
	const Texture * texture;	// texture associe
};

#endif // MATERIAL_H

