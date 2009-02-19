/*
 * Vecteur2.cpp
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Vecteur2.h"

Vecteur2::Vecteur2(f32 x, f32 y) {
	v[0] = x;
	v[1] = y;
}

Vecteur2::Vecteur2(const Vecteur2 & v) {
	this->v[0] = v[0];
	this->v[1] = v[1];
}

Vecteur2::~Vecteur2() {}

// accesseurs
f32 Vecteur2::getX() const {
	return v[0];
}
f32 Vecteur2::getY() const {
	return v[1];
}
f32 Vecteur2::operator [] (int i) const {
	return v[i];
}

// setteur
void Vecteur2::setX(f32 x) {
	v[0] = x;
}
void Vecteur2::setY(f32 y) {
	v[1] = y;
}
void Vecteur2::set(f32 x, f32 y) {
	this->v[0] = x;
	this->v[1] = y;
}
void Vecteur2::addX(f32 x) {
	v[0] += x;
}
void Vecteur2::addY(f32 y) {
	v[1] += y;
}


// operation
Vecteur2 Vecteur2::operator +(const Vecteur2 & a) const {
	return Vecteur2(v[0] + a[0], v[1] + a[1]);
}
Vecteur2 & Vecteur2::operator +=(const Vecteur2 & v) {
	this->v[0] += v[0];
	this->v[1] += v[1];
	return *this;
}
Vecteur2 Vecteur2::operator +(f32 a) const {
	return Vecteur2(v[0] + a, v[1] + a);
}
Vecteur2 & Vecteur2::operator +=(f32 a) {
	this->v[0] += a;
	this->v[1] += a;
	return *this;
}

Vecteur2 Vecteur2::operator -() const {
	return Vecteur2(-v[0], -v[1]);
}
Vecteur2 Vecteur2::operator -(const Vecteur2 & a) const {
	return Vecteur2(v[0] - a[0], v[1] - a[1]);
}
Vecteur2 & Vecteur2::operator -=(const Vecteur2 & v) {
	this->v[0] -= v[0];
	this->v[1] -= v[1];
	return *this;
}
Vecteur2 Vecteur2::operator -(f32 a) const {
	return Vecteur2(v[0] - a, v[1] - a);
}
Vecteur2 & Vecteur2::operator -=(f32 a) {
	this->v[0] -= a;
	this->v[1] -= a;
	return *this;
}

Vecteur2 Vecteur2::operator *(const Vecteur2 & a) const {
	return Vecteur2(v[0] * a[0], v[1] * a[1]);
}
Vecteur2 & Vecteur2::operator *=(const Vecteur2 & v) {
	this->v[0] *= v[0];
	this->v[1] *= v[1];
	return *this;
}
Vecteur2 Vecteur2::operator *(f32 a) const {
	return Vecteur2(v[0] * a, v[1] * a);
}
Vecteur2 & Vecteur2::operator *=(f32 a) {
	this->v[0] *= a;
	this->v[1] *= a;
	return *this;
}

Vecteur2 Vecteur2::operator /(const Vecteur2 & a) const {
	return Vecteur2(v[0] / a[0], v[1] / a[1]);
}
Vecteur2 & Vecteur2::operator /=(const Vecteur2 & v) {
	this->v[0] /= v[0];
	this->v[1] /= v[1];
	return *this;
}
Vecteur2 Vecteur2::operator /(f32 a) const {
	return Vecteur2(v[0] / a, v[1] / a);
}
Vecteur2 & Vecteur2::operator /=(f32 a) {
	this->v[0] /= a;
	this->v[1] /= a;
	return *this;
}

Vecteur2 & Vecteur2::operator = (const Vecteur2 & v) {
	this->v[0] = v[0];
	this->v[1] = v[1];
	return *this;
}

bool Vecteur2::operator == (const Vecteur2 & v) const {
	return this->v[0] == v.v[0] && this->v[1] == v.v[1];
}

Vecteur2 operator + (f32 s, const Vecteur2 & v) {return v+s;}
Vecteur2 operator - (f32 s, const Vecteur2 & v) {return -v+s;}
Vecteur2 operator * (f32 s, const Vecteur2 & v) {return v*s;}
Vecteur2 operator / (f32 s, const Vecteur2 & v) {
	return Vecteur2(s / v[0], s / v[1]);
}
