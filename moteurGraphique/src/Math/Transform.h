/*
 * Transform.h
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef _Transform_h_
#define _Transform_h_

#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "../Type.h"

class Transform {
public:

	// constructeur
	Transform(Vector3 position = Vector3::ZERO, Quaternion rotation = Quaternion::IDENTITY, Vector3 scale = Vector3::UNIT_SCALE);
	Transform(const Matrix4 & mat);
	Transform(const Transform & t);

	virtual const Transform& operator =(const Transform t);

	static const Transform IDENTITY;

	virtual void setScale(const Vector3 & s);
	virtual void setScale(f32 sx, f32 sy, f32 sz);
	virtual void setScale(f32 s);
	virtual inline const Vector3 & getScale(void) const {return scale;}
	virtual inline Vector3 & getScale(void) {return scale;}

	virtual const Quaternion& getOrientation(void) const {return rotation;}
	virtual Quaternion & getOrientation(void) {return rotation;}

	virtual inline const Vector3& getPosition(void) const {return position;}
	virtual inline Vector3 & getPosition(void) {return position;}

	// deplacer un objet
	virtual void setPosition(const Vector3 &);
	virtual void setPosition(f32 x, f32 y, f32 z);
	virtual void translate(const Vector3 &);
	virtual void translate(f32 x, f32 y, f32 z);
	virtual Transform operator + (const Vector3 &) const;
	virtual const Transform & operator += (const Vector3 &);
	virtual Transform operator - (const Vector3 &) const;
	virtual const Transform & operator -= (const Vector3 &);

	// tourner un objet
	virtual void setRotation(const Quaternion &);
	virtual void setRotation(f32 angleX, f32 angleY, f32 angleZ);
	virtual void setRotation(f32 angle, const Vector3 & axe);
	virtual void setRotation(f32 angle, f32 axeX, f32 axeY, f32 axeZ);
	virtual void rotate(const Quaternion &);
	virtual void rotate(f32 angleX, f32 angleY, f32 angleZ);
	virtual void rotate(f32 angle, const Vector3 & axe);
	virtual void rotate(f32 angle, f32 axeX, f32 axeY, f32 axeZ);

	virtual inline Vector3 toLocalSpace(const Vector3 & v) const {return getInverseMatrix() * v;}
	virtual void multCurrentMatrix(void) const;
	virtual inline Vector3 operator * (const Vector3 & v) const {return getMatrix() * v;}	// TODO optimiser
	virtual inline const Transform & operator *= (const Transform& t) {
		position = *this * t.position;
		rotation = this->getOrientation() * t.rotation;
		scale = this->scale * t.scale;
		return *this;
	}
	virtual inline Transform operator * (const Transform & t) const {
		Transform r = *this;
		r *= t;
		return r;
	}

protected:
	Matrix4 getMatrix(void) const;
	Matrix4 getInverseMatrix(void) const;

	Vector3 position;
	Quaternion rotation;
	Vector3 scale;
};

#endif
