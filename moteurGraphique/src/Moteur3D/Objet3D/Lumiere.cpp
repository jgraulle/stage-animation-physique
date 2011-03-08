/*
 * Lumiere.cpp
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Lumiere.h"

#include "../../Math/Matrix3.h"
#include "../../Outils.h"

const Couleur Lumiere::AMBIENT_DEFAUT = Couleur(0.0f, 0.0f, 0.0f, 1.0f);
const Couleur Lumiere::DIFFUSE_SPECULAR_DEFAUT = Couleur(1.0f, 1.0f, 1.0f, 1.0f);

Lumiere::Lumiere(const Vector3 & position, const Couleur & c, f32 attnConstant, f32 attnLinear, f32 attnQuadratic, u8 exposant, f32 angle, const Vector3 & direction)
: position(position), ambientColor(AMBIENT_DEFAUT), diffuseColor(c), specularColor(c), direction(direction),
angle(angle), exposant(exposant), attnConstant(attnConstant), attnLinear(attnLinear), attnQuadratic(attnQuadratic) {}

Lumiere::Lumiere(const Vector3 & position, const Couleur & ambientColor, const Couleur & diffuseColor,  const Couleur & specularColor, f32 attnConstant, f32 attnLinear, f32 attnQuadratic, u8 exposant, f32 angle, const Vector3 & direction)
: position(position), ambientColor(ambientColor), diffuseColor(diffuseColor), specularColor(specularColor), direction(direction),
angle(angle), exposant(exposant), attnConstant(attnConstant), attnLinear(attnLinear), attnQuadratic(attnQuadratic) {}

Lumiere::Lumiere(const Vector3 & position, const Quaternion & rotation)
: position(position), ambientColor(AMBIENT_DEFAUT), diffuseColor(DIFFUSE_SPECULAR_DEFAUT), specularColor(DIFFUSE_SPECULAR_DEFAUT),
angle(45.0f), exposant(0), attnConstant(1.0f), attnLinear(0.0f), attnQuadratic(0.0f) {
	f32 dir[] = {0.0f, 0.0f, -1.0f};
	Matrix3 rot;
	rotation.ToRotationMatrix(rot);
	direction.set(dir[0]*rot[0][0]+dir[1]*rot[0][1]+dir[2]*rot[0][2],
	              dir[0]*rot[1][0]+dir[1]*rot[1][1]+dir[2]*rot[1][2],
	              dir[0]*rot[2][0]+dir[1]*rot[2][1]+dir[2]*rot[2][2]);
}

Lumiere::~Lumiere() {}

const Vector3 & Lumiere::getPosition() const {return position;}
void Lumiere::setPosition(const Vector3 & p) {position = p;}

// spot
const Vector3 & Lumiere::getDirection() const {return direction;}
void Lumiere::setDirection(const Vector3 & d) {direction = d;}
void Lumiere::setDirection(const Quaternion & q) {
	f32 dir[] = {0.0f, 0.0f, -1.0f};
	Matrix3 rot;
	q.ToRotationMatrix(rot);
	direction.set(dir[0]*rot[0][0]+dir[1]*rot[0][1]+dir[2]*rot[0][2],
	              dir[0]*rot[1][0]+dir[1]*rot[1][1]+dir[2]*rot[1][2],
	              dir[0]*rot[2][0]+dir[1]*rot[2][1]+dir[2]*rot[2][2]);
}
f32 Lumiere::getAngle() const {return angle;}
void Lumiere::setAngle(f32 a){angle = a;}
u8 Lumiere::getExposant() const {return exposant;}
void Lumiere::setExposant(u8 m) {exposant = m;}

// attenuation
f32 Lumiere::getAttnConstant() const {return attnConstant;}
void Lumiere::setAttnConstant(f32 k0) {attnConstant = k0;}
f32 Lumiere::getAttnLinear() const {return attnLinear;}
void Lumiere::setAttnLinear(f32 k1) {attnLinear = k1;}
f32 Lumiere::getAttnQuadratic() const {return attnQuadratic;}
void Lumiere::setAttnQuadratic(f32 k2) {attnQuadratic = k2;}
void Lumiere::setAttn(f32 constant, f32 linear, f32 quadratic) {
	attnConstant = constant;
	attnLinear = linear;
	attnQuadratic = quadratic;
}

// afficher la lumiere
void Lumiere::display(u8 numLight) {
	{
		Disable light(GL_LIGHTING);
		Disable texture(GL_TEXTURE_2D);

		glPointSize(5.0);
		glColor3fv(diffuseColor.getData());
		glBegin(GL_POINTS);
			glVertex3fv(position.data());
		glEnd();
		glPointSize(1.0);
	}

	GLenum number = GL_LIGHT0+numLight;
	GLfloat pos[] = {position.x, position.y, position.z, 1.0f};
	glLightfv(number, GL_POSITION, pos);

	glLightfv(number, GL_AMBIENT, ambientColor.getData());
	glLightfv(number, GL_DIFFUSE, diffuseColor.getData());
	glLightfv(number, GL_SPECULAR, specularColor.getData());

	glLightf(number, GL_SPOT_EXPONENT, exposant);
	glLightf(number, GL_SPOT_CUTOFF, angle);
	glLightfv(number, GL_SPOT_DIRECTION, direction.data());

	glLightf(number, GL_CONSTANT_ATTENUATION, attnConstant);
	glLightf(number, GL_LINEAR_ATTENUATION, attnLinear);
	glLightf(number, GL_QUADRATIC_ATTENUATION, attnQuadratic);
}
