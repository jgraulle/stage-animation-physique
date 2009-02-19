/*
 * Monde2D.h
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef MONDE2D_H_
#define MONDE2D_H_

#include "Objet2D/Objet2D.h"
#include "../Erreur.h"
#include "../Type.h"

#include <ext/hash_map>
#include <map>
#include <list>
#include <string>

using namespace std;
using namespace __gnu_cxx;

class Monde2D {
public:
	// constructeur
	Monde2D(const Vecteur2 & tailleProjection);

	// destruction du monde2D et de tout les objets qu'il contient
	// les objets contenue sont desaloues
	virtual ~Monde2D();

	// modifier la taille de la projection
	virtual void setTailleProjection(const Vecteur2 &);

	// affichage de tous les objets du Monde2D
	virtual void display();

	// mise a jour de tous les objets du Monde2D
	virtual void update(float elapsed);

	// ajouter un objet 2D au monde
	// attention : cet objet ne sera pas detruit par le Monde
	virtual void add(const string & nom, int z, Objet2D * o);
	// erreur ErreurObjet2DAlreadyExist

	// enlever un objet2D du monde
	// attention : la memoire n'est pas desaloue
	virtual Objet2D * remove(const string & nom);
	// erreur ErreurObjet2DNotFind

	// acceder a un objet2D
	virtual Objet2D * get(const string & o);
	// erreur ErreurObjet2DNotFind

	// acceder a la profondeur d'un objet 2D
	virtual int getZ(const string & nom, int z) const;
	// erreur ErreurObjet2DNotFind

	// changer la profondeur d'un objet 2D
	virtual void setZ(const string & nom, int z);
	virtual void addZ(const string & nom, int z);
	// erreur ErreurObjet2DNotFind

	class ErreurObjet2DNotFind : public Erreur
	{ public:	ErreurObjet2DNotFind(string o) : Erreur("l'objet2D '"+o+"' n'a pas ete trouve dans le monde 2D") {} };
	class ErreurObjet2DAlreadyExist : public Erreur
	{ public:	ErreurObjet2DAlreadyExist(string o) : Erreur("l'objet2D '"+o+"' existe deja dans le monde 2D") {} };

private:
	// fonction de hash pour les string
	struct Hashstring {size_t operator()(const std::string& x) const {return hash<const char*>()(x.c_str());}};

	// ensemble des objets 2d indexe par profondeur
	typedef multimap<int, Objet2D*, greater<int> > Objet2DsZ;
	Objet2DsZ objet2DsZ;

	// ensemble des objets 2d indexe par le nom
	typedef hash_map<string, Objet2DsZ::iterator, Hashstring> Objet2DsNom;
	Objet2DsNom objet2DsNom;

	// matrice de projection
	Mtx44f32 projection;
	Mtx44f32 modelView;
};

#endif /* MONDE2D_H_ */
