/*
 * Vecteur2.h
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef Vecteur2_H_
#define Vecteur2_H_

#include "../Type.h"

class Vecteur2 {
	friend Vecteur2 operator + (f32, const Vecteur2 &);
	friend Vecteur2 operator - (f32, const Vecteur2 &);
	friend Vecteur2 operator * (f32, const Vecteur2 &);
	friend Vecteur2 operator / (f32, const Vecteur2 &);

public:
	// constructeur
	Vecteur2(f32 x = 0.0, f32 y = 0.0);
	Vecteur2(const Vecteur2 &);
	virtual ~Vecteur2();

	// accesseurs
	f32 getX() const;
	f32 getY() const;
	f32 operator [] (int) const;

	// modifieur
	void setX(f32);
	void setY(f32);
	void set(f32 x, f32 y);
	void addX(f32);
	void addY(f32);

	// operation
	Vecteur2 operator +(const Vecteur2 &) const;
	Vecteur2 & operator +=(const Vecteur2 &);
	Vecteur2 operator +(f32) const;
	Vecteur2 & operator +=(f32);

	Vecteur2 operator -() const;
	Vecteur2 operator -(const Vecteur2 &) const;
	Vecteur2 & operator -=(const Vecteur2 &);
	Vecteur2 operator -(f32) const;
	Vecteur2 & operator -=(f32);

	Vecteur2 operator *(const Vecteur2 &) const;
	Vecteur2 & operator *=(const Vecteur2 &);
	Vecteur2 operator *(f32) const;
	Vecteur2 & operator *=(f32);

	Vecteur2 operator /(const Vecteur2 &) const;
	Vecteur2 & operator /=(const Vecteur2 &);
	Vecteur2 operator /(f32) const;
	Vecteur2 & operator /=(f32);

	Vecteur2 & operator = (const Vecteur2 &);
	bool operator == (const Vecteur2 &) const;

private:
	f32 v[2];
};

#endif /* Vecteur2_H_ */
