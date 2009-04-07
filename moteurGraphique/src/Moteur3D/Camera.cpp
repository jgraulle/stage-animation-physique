/*
 * Camera.cpp
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Camera.h"

#include "../Outils.h"

#include <GL/gl.h>

// constructeur
Camera::Camera(const Vector3 & centreObservation, f32 rotX, f32 rotY, f32 distance)
:centre(centreObservation), rx(rotX), ry(rotY), dist(distance) {}

// destructeur
Camera::~Camera() {}

// accesseur sur le centre d'observation
const Vector3 & Camera::getCentreObser() { return centre;}

// reinitialiser le centre d'observation
void Camera::setCentreObser(const Vector3 & v) {centre = v;}

// deplacer le centre d'observation en ajoutant un vecteur
void Camera::moveCentreObser(const Vector3 & v) {
	f32 angle = rx * M_PI / 180.0;	// conversion de l'angle en radian
	f32 cosAngle = cos(angle);
	f32 sinAngle = sin(angle);
	centre.x = centre.x+v.x*cosAngle - v.z*sinAngle;
	centre.z = centre.z+v.x*sinAngle + v.z*cosAngle;
	centre.y = centre.y+v.y;
}

// accesseur sur la rotation gauche-droite de la camera
f32 Camera::getRotX() {return rx;}

// accesseur sur la rotation haut-bas de la camera
f32 Camera::getRotY() {return ry;}

// reinitialiser la rotation de de la camera
void Camera::setRot(f32 gaucheDroite, f32 hautBas) {
	rx = (f32)((((int) gaucheDroite)+180)%360 - 180);
	ry = (f32)(((int)(hautBas)+180)%360 - 180);
}

// modifier le rotation en ajoutant un f32 a chaque axe
void Camera::addRot(f32 gaucheDroite, f32 hautBas) {
	addRotX(gaucheDroite);
	addRotY(hautBas);
}

// modifier le rotation gauche-droite en ajoutant un f32
void Camera::addRotX(f32 a) {
	rx += a;
	if (rx>180.0)
		rx-=360.0;
	if (rx<-180.0)
		rx+=360.0;
}

// modifier le rotation haut-bas en ajoutant un f32
void Camera::addRotY(f32 a) {
	ry += a;
	if (ry>180.0)
		ry-=360.0;
	if (ry<-180.0)
		ry+=360.0;
}

// accesseur sur la distance de la camera au centre d'observation
f32 Camera::getDistance() {return dist;}

// reinitialiser la distance de la camera au centre d'observation
void Camera::setDistance(f32 d) {
	if (d>0)
		dist = d;
	else
		throw DistanceNegative();
}

// modifier la distance en ajoutant un f32
void Camera::addDistance(f32 d) {
	dist += d;
	if (dist<0.0)
		dist=0.0;
}

// faire les transformations openGL necessaire pour bien placer la camera
void Camera::positionner() {
    // deplacer la camera a la distance du point fixe
    glTranslated(0.0, 0.0, -getDistance());
    // tourner la camera
    glRotated((f32)getRotY(), 1.0, 0.0, 0.0);
    glRotated((f32)getRotX(), 0.0, 1.0, 0.0);

	{
		// afficher le centre de la camera
		Disable lumiere(GL_LIGHTING);
		Disable texture(GL_TEXTURE_2D);
		Disable profondeur(GL_DEPTH_TEST);
		glBegin(GL_POINTS);
		glVertex3fv(Vector3::ZERO);
		glEnd();
	}
    // definir les coordonnes du point fixe
    glTranslated(-getCentreObser().x, -getCentreObser().y, -getCentreObser().z);
}
