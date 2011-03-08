/*
 * Camera.h
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "../Math/Vector3.h"
#include "../Type.h"
#include "../Erreur.h"

class Camera {
public:

	// constructeur
	Camera(const Vector3 & centreObservation=Vector3(0.0,0.0,0.0),
			f32 rotX=0.0, f32 rotY=0.0, f32 distance=0.0);

	// destructeur
	virtual ~Camera();

	// accesseur sur le centre d'observation
	const Vector3 & getCentreObser() const;

	// reinitialiser le centre d'observation
	void setCentreObser(const Vector3 &);

	// deplacer le centre d'observation par rapport a un vecteur deplacement
	// ou l'axe des Z pointe vers la camera
	// (prend en compte la rotation de gauche-droite de la camera)
	void moveCentreObser(const Vector3 &);

	// accesseur sur la rotation gauche-droite de la camera
	f32 getRotX() const;

	// accesseur sur la rotation haut-bas de la camera
	f32 getRotY() const;

	// reinitialiser la rotation de de la camera en degres
	void setRot(f32 gaucheDroite, f32 hautBas);
	// prend le modulo 360 des angles

	// modifier le rotation en ajoutant un f32 a chaque axe
	void addRot(f32 gaucheDroite, f32 hautBas);

	// modifier le rotation gauche-droite en ajoutant un f32
	void addRotX(f32);

	// modifier le rotation haut-bas en ajoutant un f32
	void addRotY(f32);

	// accesseur sur la distance de la camera au centre d'observation
	f32 getDistance() const;

	// reinitialiser la distance de la camera au centre d'observation
	void setDistance(f32);
	// erreur DistanceNegative si negatif

	// erreur si la distance entre la camera au centre d'observation est negative
	class DistanceNegative : public Erreur
	{ public: DistanceNegative():Erreur("Impossible de donnée une distance négative pour la camera"){} };

	// modifier la distance en ajoutant un f32
	void addDistance(f32);
	// si negatif sera ramener a 0.0

	// faire les transformations openGL necessaire pour bien placer la camera
	void positionner() const;

	// dessiner le centre de la camera
	void display() const;

private:
	Vector3 centre;	// centre d'observation de la camera
	f32 rx;			// rotation gauche-droite en °
	f32 ry;			// rotation haut-bas en °
	f32 dist; 		// distance de la camera au centre d'observation
};

#endif /* CAMERA_H_ */
