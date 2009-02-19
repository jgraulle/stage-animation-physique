/*
 * Objet3D.h
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef OBJET3D_H_
#define OBJET3D_H_

#include "../Material.h"
#include "../../Ressource/Mesh.h"
#include "../../Math/Transform.h"

#include <string>

using namespace std;

class Objet3D {
public:

	// constructeur
	Objet3D(Material material, const Mesh * mesh, Transform transform = Transform());
	Objet3D(Material material, const string & mesh, Transform transform = Transform());

	// destructeur par defaut
	virtual ~Objet3D();

	// affichage
	virtual void display() const;

	// fonction de mise a jour de l'objet
	virtual void update(f32 elapsed);

	// calcule et renvoie la matrice correspondant a la position de la camera
	virtual const Transform & getTransform() const;
	virtual Transform & getTransform();

	// retourne le material appliquer a l'objet
	virtual Material & getMaterial() {return material;}
	virtual const Material & getMaterial() const {return material;}

	// enlever du monde
	virtual void retirerDuMonde();

	// remettre dans le monde
	virtual void remettreDansMonde();

	// vrai si l'objet est dan le monde
	virtual bool isDansLeMonde() const {return estDansLeMonde;}

private:
	Transform transform;
	Material material;
	const Mesh * mesh;
	bool estDansLeMonde;
};

#endif /*OBJET3D_H_*/
