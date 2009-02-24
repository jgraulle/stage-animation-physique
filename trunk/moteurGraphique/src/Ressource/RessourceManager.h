/*
 * RessourceManager.h
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef RESSOURCEMANAGER_H_
#define RESSOURCEMANAGER_H_

#include "../Erreur.h"

#include <ext/hash_map>

using namespace __gnu_cxx;
using namespace std;

// fonction de hash pour les string
struct HashString {size_t operator()(const std::string& x) const {return hash<const char*>()(x.c_str());}};

//typedef Texture T;
template <class T, const char * ressourceName>
class RessourceManager {
private:
	typedef typename hash_map<string, const T *, HashString>::iterator iterator;
	typedef typename hash_map<string, const T *, HashString>::const_iterator const_iterator;

public:
	// retourne la seule instance du manager de ressource
	static RessourceManager * getInstance() {
		static RessourceManager instance;
		return &instance;
	}

	// destructeur
	virtual ~RessourceManager() {
		for(	iterator it = ressources.begin() ;
				it != ressources.end() ;
				it++) {
			delete it->second;
		}
		ressources.clear();
	}

	// ajout d'une ressource
	virtual const T * add(const T * r) {
		// chercher ce nom dans la map des ressources
		iterator it = ressources.find(r->getName());
		// si cette ressource existe deja
		if (it != ressources.end())
			// lever une exception
			throw ErreurRessourceAlreadyExist(r->getName());
		// si non
		// l'ajouter
		ressources[r->getName()] = r;
		return r;
	}

	// enlever une ressource
	virtual void remove(const string & name) {
		// chercher l'element
		iterator it = ressources.find(name);
		// s'il n'a pas ete trouve
		if (it == ressources.end())
			// lever une exception
			throw ErreurRessourceNotFind(name);
		// si non
		// supprimer l'element de la map
		delete it->second;
		ressources.erase(it);
	}

	// acceder a une ressource
	virtual const T * get(const string & name) const {
		const_iterator it = ressources.find(name);
		if (it == ressources.end())
			throw ErreurRessourceNotFind(name);
		return it->second;
	}

	class ErreurRessourceNotFind : public Erreur
	{ public:	ErreurRessourceNotFind(string name) : Erreur(string("la ")+ressourceName+" '"+name+"' n'a pas ete trouve dans le manager des " + ressourceName + "s") {} };
	class ErreurRessourceAlreadyExist : public Erreur
	{ public:	ErreurRessourceAlreadyExist(string name) : Erreur(string("la ")+ressourceName+" '"+name+"' existe deja dans le manager des " + ressourceName + "s") {} };

private:
	// constructeur
	RessourceManager() {}

	// ensemble des objets 2d indexe par le nom
	hash_map<string, const T *, HashString> ressources;
};

#include "Image.h"
#include "Texture.h"
#include "Mesh.h"

extern const char imagesManagerName[];
typedef RessourceManager<Image, imagesManagerName> ImagesManager;
extern const char texturesManagerName[];
typedef RessourceManager<Texture, texturesManagerName> TexturesManager;
extern const char meshManagerName[];
typedef RessourceManager<Mesh, meshManagerName> MeshManager;

#endif /* RESSOURCEMANAGER_H_ */
