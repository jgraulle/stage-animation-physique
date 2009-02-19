/*
 * Monde3D.h
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef MONDE_H
#define MONDE_H

#include <list>
#include <map>
#include <ext/hash_map>
#include <string>
#include <algorithm>

#include "Objet3D/Objet3D.h"
#include "Objet3D/Lumiere.h"
#include "Camera.h"
#include "../Erreur.h"

using namespace __gnu_cxx;
using namespace std;

class Monde3D {
public:

	static const int NBR_MAX_LUMIERE = 8;

	// constructeur
	Monde3D(const Vecteur2 & tailleProjection);

	// destructeur
	// destruction de tous les objets (Objets3D, lumieres) contenus
	virtual ~Monde3D();

	// modifier la matrice de projection
	virtual void setProjection(f32 angle = 45.0f, f32 ratio = 1.0f, f32 zMin = 0.5f, f32 zMax = 300.0f);

	// affichage de tous les objets du Monde
	virtual void display();

	// mise a jour de tous les objets du Monde
	virtual void update(float elapsed);

	// ajouter un objet 3D au monde
	virtual void add(const string & nom, Objet3D * o, bool transparent = false);
	// erreur ErreurObjet3DAlreadyExist

	// enlever un objet3D du monde
	virtual Objet3D * remove(const string & nom);
	// erreur ErreurObjet3DNotFind

	// acceder a un un objet3D
	virtual Objet3D * get(const string & nom);
	// erreur ErreurObjet3DNotFind

	// ajoute une lumiere au monde 3D
	virtual void add(const string & nom, Lumiere * l);
	// erreur ErreurTropLumiere si il y a trop de lumiere
	// erreur ErreurLumiereAlreadyExist

	// enlever une lumiere du monde et desalouer la memoire
	virtual Lumiere * removeLumiere(const string & nom);
	// erreur ErreurLumiereNotFind

	// acceder a une lumiere
	virtual Lumiere * getLumiere(const string & nom);
	// erreur ErreurLumiereNotFind

	// acceder a la camera
	virtual Camera * getCamera();

	class ErreurObjet3DNotFind : public Erreur
	{ public:	ErreurObjet3DNotFind(string o) : Erreur("l'objet3D '"+o+"' n'a pas ete trouve dans le monde") {} };
	class ErreurObjet3DAlreadyExist : public Erreur
	{ public:	ErreurObjet3DAlreadyExist(string o) : Erreur("l'objet3D '"+o+"' existe deja dans le monde 3D") {} };

	class ErreurTropLumiere : public Erreur
	{ public:	ErreurTropLumiere() : Erreur("erreur le monde ne peut pas contenir plus de 8 lumieres") {} };
	class ErreurLumiereNotFind : public Erreur
	{ public:	ErreurLumiereNotFind(string l) : Erreur("la lumiere '"+l+"' n'a pas ete trouve dans le monde") {} };
	class ErreurLumiereAlreadyExist : public Erreur
	{ public:	ErreurLumiereAlreadyExist(string l) : Erreur("la lumiere '"+l+"' existe deja dans le monde 3D") {} };

private:
	// fonction de hash pour les string
	struct HashString {size_t operator()(const std::string& x) const {return hash<const char*>()(x.c_str());}};

	typedef list<Objet3D*> Objet3Ds;
	// liste des objets 3d opaques
	Objet3Ds objet3DsOpaque;
	// liste des objets 3d transparent
	Objet3Ds objet3DsTransparent;

	// liste des objets 3d trier par nom pour la recherche
	typedef hash_map<string, pair<Objet3Ds::iterator, bool>, HashString> Objet3DsNom;
	Objet3DsNom objet3DsNom;

	// tableau des lumieres
	// une map suffit car il y a au maximun 10 lumieres
	typedef map<string, Lumiere*> Lumieres;
	Lumieres lumieres;

	// camera
	Camera camera;

	// matrice de projection
	Mtx44f32 projection;
};

#endif // MONDE_H
