/*
 * Material.cpp
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Material.h"

#include "../Ressource/RessourceManager.h"

using namespace std;

const Couleur Material::AMBIENT_DEFAUT = Couleur(0.2f, 0.2f, 0.2f, 1.0f);
const Couleur Material::DIFFUSE_DEFAUT = Couleur(0.8f, 0.8f, 0.8f, 1.0f);
const Couleur Material::SPECULAR_DEFAUT = Couleur(0.8f, 0.8f, 0.8f, 1.0f);
const Couleur Material::EMISSION_DEFAUT = Couleur(0.0f, 0.0f, 0.0f, 1.0f);
const u8 Material::SHININESS_DEFAUT = 0;

Material::Material(const Texture * texture, const Couleur & ambient, const Couleur & diffuse, const Couleur & specularColor, const Couleur & emissionColor, u8 shininess)
: ambientColor(ambient), diffuseColor(diffuse), specularColor(specularColor), emissionColor(emissionColor), shininess(shininess), texture(texture) {
	assert(shininess<=128);
}

Material::Material(const string & texture, const Couleur & ambient, const Couleur & diffuse, const Couleur & specularColor, const Couleur & emissionColor, u8 shininess)
: ambientColor(ambient), diffuseColor(diffuse), specularColor(specularColor), emissionColor(emissionColor), shininess(shininess), texture(TexturesManager::getInstance()->get(texture)) {
	assert(shininess<=128);
}

// constructeur de copie
Material::Material(const Material & mat)
: ambientColor(mat.ambientColor), diffuseColor(mat.diffuseColor), specularColor(mat.specularColor), emissionColor(mat.emissionColor), shininess(mat.shininess), texture(mat.texture) {}


Material::~Material() {
}

void Material::apply() const {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor.getData());
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor.getData());
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor.getData());
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissionColor.getData());
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	assert(texture!=NULL);
	texture->apply();
}
