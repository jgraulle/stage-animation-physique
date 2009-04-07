/*
 * Transform.cpp
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Transform.h"

#include <GL/gl.h>

Transform::Transform(const Vector3 & position, const Quaternion & rotation, const Vector3 & scale)
: position(position), rotation(rotation), scale(scale) {}

Transform::Transform(const Matrix4 & mat)
: scale(Vector3::UNIT_SCALE) {
	position = Vector3(mat[0][3], mat[1][3], mat[2][3]);
	Matrix3 rot;
	mat.extract3x3Matrix(rot);
	rotation.FromRotationMatrix(rot);
}

Transform::Transform(const Transform & t) {
	position = t.getPosition();
	scale = t.getScale();
	rotation = t.getOrientation();
}

Transform & Transform::operator =(const Transform & t) {
	position = t.getPosition();
	scale = t.getScale();
	rotation = t.getOrientation();
	return *this;
}
// TODO const Transform Transform::IDENTITY(Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE);
const Transform Transform::IDENTITY(Vector3(0.0, 0.0, 0.0), Quaternion(1.0, 0.0, 0.0, 0.0), Vector3(1.0, 1.0, 1.0));

void Transform::setScale(const Vector3& vScale) {
	scale = vScale;
}
void Transform::setScale(float sx, float sy, float sz) {
	scale = Vector3(sx, sy, sz);
}
void Transform::setScale(float s) {
	scale = Vector3(s, s, s);
}

// deplacer un objet
void Transform::setPosition(const Vector3 & position) {
	this->position = position;
}
void Transform::setPosition(f32 x, f32 y, f32 z) {
	position = Vector3(x, y, z);
}
void Transform::translate(const Vector3 & v) {
	this->position += v;
}
void Transform::translate(f32 x, f32 y, f32 z) {
	this->position += Vector3(x, y, z);
}
Transform Transform::operator + (const Vector3 & v) const {
	Transform t = *this;
	t.translate(v);
	return t;
}
const Transform & Transform::operator += (const Vector3 & v) {
	translate(v);
	return *this;
}
Transform Transform::operator - (const Vector3 & v) const {
	Transform t = *this;
	t.translate(-v);
	return t;
}
const Transform & Transform::operator -= (const Vector3 & v) {
	translate(-v);
	return *this;
}

// tourner la camera
void Transform::setRotation(const Quaternion & rotation) {
	this->rotation = rotation;
}
void Transform::setRotation(f32 angleX, f32 angleY, f32 angleZ) {
	rotation = Quaternion(angleX, angleY, angleZ);
}
void Transform::setRotation(f32 angle, const Vector3 & axe) {
	rotation = Quaternion::IDENTITY;
	rotation.FromAngleAxis(angle, axe);
}
void Transform::setRotation(f32 angle, f32 axeX, f32 axeY, f32 axeZ) {
	rotation = Quaternion::IDENTITY;
	rotation.FromAngleAxis(angle, Vector3(axeX, axeY, axeZ));
}
void Transform::rotate(const Quaternion & q) {
	rotation = rotation * q;
}
void Transform::rotate(f32 angleX, f32 angleY, f32 angleZ) {
	Quaternion qx = Quaternion::IDENTITY;
	qx.FromAngleAxis(angleX, Vector3::UNIT_X);
	Quaternion qy = Quaternion::IDENTITY;
	qy.FromAngleAxis(angleY, Vector3::UNIT_Y);
	Quaternion qz = Quaternion::IDENTITY;
	qz.FromAngleAxis(angleZ, Vector3::UNIT_Z);
	rotation = rotation * qx * qy * qz;
}
void Transform::rotate(f32 angle, const Vector3 & axe) {
	Quaternion r = Quaternion::IDENTITY;
	r.FromAngleAxis(angle, axe);
	rotation = rotation * r;
}
void Transform::rotate(f32 angle, f32 axeX, f32 axeY, f32 axeZ) {
	Quaternion r = Quaternion::IDENTITY;
	r.FromAngleAxis(angle, Vector3(axeX, axeY, axeZ));
	rotation = rotation * r;
}

Matrix4 Transform::getMatrix(void) const {
	Matrix4 mat(Matrix4::IDENTITY);
	Matrix3 rotMat;
	rotation.ToRotationMatrix(rotMat);
	mat = rotMat;
	mat[0][0] *= scale.x;
	mat[0][1] *= scale.x;
	mat[0][2] *= scale.x;
	mat[1][0] *= scale.y;
	mat[1][1] *= scale.y;
	mat[1][2] *= scale.y;
	mat[2][0] *= scale.z;
	mat[2][1] *= scale.z;
	mat[2][2] *= scale.z;
	mat.setTrans(position);
	return mat;
}

void Transform::applyGL(void) const {
	glTranslatef(position.x, position.y, position.z);
	float a;
	Vector3 axis;
	rotation.ToAngleAxis(a, axis);
	glRotatef(a * 180. / M_PI, axis.x, axis.y, axis.z);
	glScalef(scale.x, scale.y, scale.z);
}

Vector3 Transform::operator * (const Vector3 & v) const {
	// TODO tester le scale
	return (rotation * v + position) * scale;
//	return getMatrix() * v;
}

const Transform & Transform::operator *= (const Transform& t) {
	position = *this * t.position;
	rotation = this->getOrientation() * t.rotation;
	scale = this->scale * t.scale;
	return *this;
}

Transform Transform::operator * (const Transform & t) const {
	Transform r = *this;
	r *= t;
	return r;
}

Transform Transform::inverse(void) const  {
	// TODO tester le scale
	Quaternion ri = rotation.Inverse();
	return Transform(ri * (-position), ri, Vector3(1.0f/scale.x, 1.0f/scale.y, 1.0f/scale.z));
}

ostream & operator << ( ostream & o, const Transform & t) {
    o << "[" << t.position << ", " << t.rotation << ", " << t.scale << "]";
    return o;
}
