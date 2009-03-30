/*
 * Monde3D.cpp
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Monde3D.h"

#include "Objet3D/Objet3D.h"
#include "../Erreur.h"
#include "../Moteur.h"

// constructeur avec definition de la taille de la zone visionner par la camera
Monde3D::Monde3D(const Vecteur2 & tailleProjection) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, tailleProjection.getX()/tailleProjection.getY(), 0.5f, 3000.0f);
	glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat*)projection);
	glMatrixMode(GL_MODELVIEW);
}

// destructeur
// destruction de tous les objets contenus
Monde3D::~Monde3D() {
	// Destruction des Objet3Ds opaques
	for(	Objet3Ds::iterator it = objet3DsOpaque.begin() ;
			it != objet3DsOpaque.end() ;
			it++) {
		delete (*it);
	}
	objet3DsOpaque.clear();
	// Destruction des Objet3Ds transparent
	for(	Objet3Ds::iterator it = objet3DsTransparent.begin() ;
			it != objet3DsTransparent.end() ;
			it++) {
		delete (*it);
	}
	objet3DsTransparent.clear();
	// vidage de la liste des objets tries par nom
	objet3DsNom.clear();
	// Destruction des lumieres
	for(	Lumieres::iterator it = lumieres.begin();
			it != lumieres.end();
			it++) {
		delete (*it).second;
	}
	lumieres.clear();
}

// modifier la matrice de projection
void Monde3D::setProjection(f32 angle, f32 ratio, f32 zMin, f32 zMax) {
	f32 ratio2 = Moteur::getInstance()->getTailleEcran().getX()/Moteur::getInstance()->getTailleEcran().getY();
	ratio2 *= ratio;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angle, ratio2, zMin, zMax);
	glGetFloatv(GL_PROJECTION_MATRIX, (GLfloat*)projection);
	glMatrixMode(GL_MODELVIEW);
}

// ajouter un objet 3D au monde
void Monde3D::add(const string & nom, Objet3D * o, bool transparent) {
	// chercher ce nom dans la table de hash des objet3D
	Objet3DsNom::iterator itn=objet3DsNom.find(nom);
	// si cet objet existe deja
	if (itn != objet3DsNom.end())
		// lever une exception
		throw ErreurObjet3DAlreadyExist(nom);
	// si non => l'ajouter
	Objet3Ds::iterator it;
	// si l'objet est transparent
	if (transparent)
		// l'ajouter en fin de liste des objets transparent
		it = objet3DsTransparent.insert(objet3DsTransparent.end(), o);
	// si non
	else
		// l'ajouter en fin de liste des objets opaques
		it = objet3DsOpaque.insert(objet3DsOpaque.end(), o);
	objet3DsNom[string(nom)] = make_pair(it, transparent);
}

// enlever un objet3D du monde
Objet3D * Monde3D::remove(const string & nom) {
	// chercher l'element
	Objet3DsNom::iterator itn = objet3DsNom.find(nom);
	// s'il n'a pas ete trouve
	if (itn == objet3DsNom.end())
		// lever une exception
		throw ErreurObjet3DNotFind(nom);
	// si non
	Objet3D * o = *(itn->second.first);
	// supprimer l'element de la list
	// si l'objet est transparent
	if (itn->second.second)
		objet3DsTransparent.erase(itn->second.first);
	// si non
	else
		objet3DsOpaque.erase(itn->second.first);
	// supprimer l'element de la hash_map trier par nom
	objet3DsNom.erase(itn);
	return o;
}

// acceder a un un objet3D
Objet3D * Monde3D::get(const string & nom) {
	// chercher l'element
	Objet3DsNom::iterator it = objet3DsNom.find(nom);
	// s'il n'a pas ete trouve
	if (it == objet3DsNom.end())
		// lever une exception
		throw ErreurObjet3DNotFind(nom);
	// si non => retourner l'objet
	return *(it->second.first);
}

// ajoute une lumiere au monde 3D
void Monde3D::add(const string & nom, Lumiere * l) {
	// si il y a deja 8 lumieres
	if (lumieres.size()>=8)
		// c'est le maximum que la carte 3d peut gerer => lever une exception
		throw ErreurTropLumiere();
	// chercher ce nom dans la table de hash des objet3D
	Lumieres::iterator it = lumieres.find(nom);
	// si cet objet existe deja
	if (it != lumieres.end())
		// lever une exception
		throw ErreurLumiereAlreadyExist(nom);
	// si non => l'ajouter
	lumieres[string(nom)] = l;
}

Lumiere * Monde3D::removeLumiere(const string & nom) {
	Lumieres::iterator it = lumieres.find(nom);
	if (it == lumieres.end())
		throw ErreurLumiereNotFind(nom);
	Lumiere * l = it->second;
	lumieres.erase(it);
	return l;
}

// acceder a une lumiere
Lumiere * Monde3D::getLumiere(const string & nom) {
	Lumieres::iterator it = lumieres.find(nom);
	if (it == lumieres.end())
		throw ErreurLumiereNotFind(nom);
	return  it->second;
}

// affichage de tous les objets du Monde
void Monde3D::display() {
	// passage en mode 3D
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat*)projection);
	glMatrixMode(GL_MODELVIEW);

	//////////////////////////////////////
	// Camera                           //
	//////////////////////////////////////
	camera.positionner();

	//////////////////////////////////////
	// affichage de toutes les lumieres //
	//////////////////////////////////////
	int numeroLumiere = 0;
	for(	Lumieres::iterator it = lumieres.begin();
			it != lumieres.end();
			it++) {
		it->second->display(numeroLumiere);
		glEnable(GL_LIGHT0+numeroLumiere);
		numeroLumiere++;
	}

	/////////////////////////////////////////////
	// Affichage de tous les objets 3D opaques //
	/////////////////////////////////////////////
	for(	Objet3Ds::iterator it = objet3DsOpaque.begin();
			it != objet3DsOpaque.end();
			it++) {
		if ((*it)->isDansLeMonde()) {
			// placer l'objet dans le monde
			glPushMatrix();
			(*it)->getTransform().multCurrentMatrix();
			// afficher l'objet
			(*it)->display();
			glPopMatrix();
		}
	}

	/////////////////////////////////////////////////
	// Affichage de tous les objets 3D transparent //
	/////////////////////////////////////////////////
	// desactiver l'ecriture du test de profondeur
//	glDepthMask(GL_FALSE);

	// Affichage de tous les objets 3D transparent
	for(	Objet3Ds::iterator it = objet3DsTransparent.begin();
			it != objet3DsTransparent.end();
			it++) {
		if ((*it)->isDansLeMonde()) {
			glPushMatrix();
			// placer l'objet dans le monde
			(*it)->getTransform().multCurrentMatrix();
			// afficher l'objet
			(*it)->display();
			glPopMatrix();
		}
	}
//	glDepthMask(GL_TRUE);
}

// mise a jour de tous les objets du Monde
void Monde3D::update(float elapsed) {
	for(	Objet3Ds::iterator it = objet3DsOpaque.begin() ;
			it != objet3DsOpaque.end() ;
			it++) {
		(*it)->update(elapsed);
	}
	for(	Objet3Ds::iterator it = objet3DsTransparent.begin() ;
			it != objet3DsTransparent.end() ;
			it++) {
		(*it)->update(elapsed);
	}
}

// acceder a la camera
Camera * Monde3D::getCamera() {
	return &camera;
}
