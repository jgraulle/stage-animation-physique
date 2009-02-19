/*
 * Objet3D.cpp
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Objet3D.h"

#include "../../Ressource/RessourceManager.h"

// constructeur par defaut
Objet3D::Objet3D(Material material, const Mesh * mesh, Transform transform)
: transform(transform), material(material), mesh(mesh), estDansLeMonde(true) {}

Objet3D::Objet3D(Material material, const string & mesh, Transform transform)
: transform(transform), material(material), mesh(MeshManager::getInstance()->get(mesh)), estDansLeMonde(true) {}

// destructeur
Objet3D::~Objet3D() {}

// affichage
void Objet3D::display() const {
	// appliquer le material
	getMaterial().apply();
	// dessiner la mesh
	mesh->display();
}

// fonction de mise a jour de l'objet
void Objet3D::update(f32 elapsed) {
}

// enlever du monde
void Objet3D::retirerDuMonde() {
	estDansLeMonde = false;
}

// remettre dans le monde
void Objet3D::remettreDansMonde() {
	estDansLeMonde = true;
}

// calcule et renvoie la matrice correspondant a la position de l'objet 3D
const Transform & Objet3D::getTransform() const {
	return transform;
}

// calcule et renvoie la matrice correspondant a la position de l'objet 3D
Transform & Objet3D::getTransform() {
	return transform;
}
