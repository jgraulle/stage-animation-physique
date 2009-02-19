/*
 * Monde2D.cpp
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Monde2D.h"

#include "../Outils.h"

Monde2D::Monde2D(const Vecteur2 & tailleProjection) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, tailleProjection.getX(), tailleProjection.getY(), 0.0, 0.0, 300.0);
	glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat*)projection);
	glMatrixMode(GL_MODELVIEW);
}

Monde2D::~Monde2D() {
	// vider les listes sans desalouer les objets
	objet2DsZ.clear();
	objet2DsNom.clear();
}

// modifier la taille de la projection
void Monde2D::setTailleProjection(const Vecteur2 & tailleProjection) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, tailleProjection.getX(), tailleProjection.getY(), 0.0, 0.0, 300.0);
	glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat*)projection);
	glMatrixMode(GL_MODELVIEW);
}

// affichage de tous les objets du Monde
void Monde2D::display() {
	// desactiver les lumieres
	Disable lumiere(GL_LIGHTING);
	// desactiver le test de profondeur
	Disable profondeur(GL_DEPTH_TEST);
	// mettre par defaut la couleur blanc
	glColor3fv(Couleur::BLANC.getData());
	// passer en mode 2D
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat*)projection);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Affichage de tous les objets 2D
	for(	Objet2DsZ::iterator it = objet2DsZ.begin() ;
			it != objet2DsZ.end() ;
			it++) {
		if (it->second->isDisplay()) {
			// afficher l'objet
			it->second->display();
		}
	}
}

// mise a jour de tous les objets du Monde
void Monde2D::update(float elapsed) {
	// actualiser tous les objets 2D
	for(	Objet2DsZ::iterator it = objet2DsZ.begin() ;
			it != objet2DsZ.end() ;
			it++) {
		// actualiser l'objet
		it->second->update(elapsed);
	}
}

// ajouter un objet 2D au monde
void Monde2D::add(const string & nom, int z, Objet2D * o) {
	// chercher ce nom dans la table de hash des objet2D
	Objet2DsNom::iterator itn = objet2DsNom.find(nom);
	// si cet objet existe deja
	if (itn != objet2DsNom.end())
		// lever une exception
		throw ErreurObjet2DAlreadyExist(nom);
	// si non => l'ajouter
	Objet2DsZ::iterator itz = objet2DsZ.insert(make_pair(z, o));
	objet2DsNom[string(nom)] = itz;
}

// enlever un objet2D du monde
Objet2D * Monde2D::remove(const string & nom) {
	// chercher l'element
	Objet2DsNom::iterator it = objet2DsNom.find(nom);
	// s'il n'a pas ete trouve
	if (it == objet2DsNom.end())
		// lever une exception
		throw ErreurObjet2DNotFind(nom);
	// si non
	Objet2D * o = it->second->second;
	// supprimer l'element de la map trier par z
	objet2DsZ.erase(it->second);
	// supprimer l'element de la map trier par nom
	objet2DsNom.erase(it);
	return o;
}

// acceder a un objet2D
Objet2D * Monde2D::get(const string & nom) {
	Objet2DsNom::iterator it = objet2DsNom.find(nom);
	if (it == objet2DsNom.end())
		throw ErreurObjet2DNotFind(nom);
	return it->second->second;
}

// acceder a la profondeur d'un objet 2D
int Monde2D::getZ(const string & nom, int z) const {
// erreur ErreurObjet2DNotFind
	// recherche l'objet en question
	Objet2DsNom::const_iterator it = objet2DsNom.find(nom);
	if (it == objet2DsNom.end())
		throw ErreurObjet2DNotFind(nom);
	return it->second->first;
}

// changer la profondeur d'un objet 2D
void Monde2D::setZ(const string & nom, int z) {
// erreur ErreurObjet2DNotFind
	// recherche l'objet en question
	Objet2DsNom::iterator it = objet2DsNom.find(nom);
	if (it == objet2DsNom.end())
		throw ErreurObjet2DNotFind(nom);
	Objet2D * o = it->second->second;
	// l'enlever de la map trier par z
	objet2DsZ.erase(it->second);
	// le rajouter
	Objet2DsZ::iterator itz = objet2DsZ.insert(make_pair(z, o));
	// mettre a jour le pointeur contenu dans la map trier par nom
	it->second = itz;
}

// changer la profondeur d'un objet 2D
void Monde2D::addZ(const string & nom, int z) {
// erreur ErreurObjet2DNotFind
	// recherche l'objet en question
	Objet2DsNom::iterator it = objet2DsNom.find(nom);
	if (it == objet2DsNom.end())
		throw ErreurObjet2DNotFind(nom);
	Objet2D * o = it->second->second;
	int newZ = it->second->first + z;
	// l'enlever de la map trier par z
	objet2DsZ.erase(it->second);
	// le rajouter
	Objet2DsZ::iterator itz = objet2DsZ.insert(make_pair(newZ, o));
	// mettre a jour le pointeur contenu dans la map trier par nom
	it->second = itz;
}
