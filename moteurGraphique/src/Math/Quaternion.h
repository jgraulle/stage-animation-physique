/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://ogre.sourceforge.net/

Copyright © 2000-2002 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/
// NOTE THAT THIS FILE IS BASED ON MATERIAL FROM:

// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000, All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#ifndef __Quaternion_H__
#define __Quaternion_H__

#include <iostream>

class Vector3;
class Matrix3;

/** Implementation of a Quaternion, i.e. a rotation around an axis.
*/
class Quaternion
{
public:
    Quaternion (
        float fW = 1.0,
        float fX = 0.0, float fY = 0.0, float fZ = 0.0);
    Quaternion (const Quaternion& rkQ);

    void FromRotationMatrix (const Matrix3& kRot);
    void ToRotationMatrix (Matrix3& kRot) const;
    void FromAngleAxis (const float& rfAngle, const Vector3& rkAxis);
    void ToAngleAxis (float& rfAngle, Vector3& rkAxis) const;
    void FromAxes (const Vector3* akAxis);
    void FromAxes (const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
    void ToAxes (Vector3* akAxis) const;
    void ToAxes (Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;

    Quaternion& operator= (const Quaternion& rkQ);
    Quaternion operator+ (const Quaternion& rkQ) const;
    Quaternion operator- (const Quaternion& rkQ) const;
    Quaternion operator* (const Quaternion& rkQ) const;
    Quaternion operator* (float fScalar) const;
    friend Quaternion operator* (float fScalar,
        const Quaternion& rkQ);
    Quaternion operator- () const;
    bool operator== (const Quaternion& rhs) const;

    // functions of a quaternion
    float Dot (const Quaternion& rkQ) const;  // dot product
    float Norm () const;  // squared-length
    Quaternion Inverse () const;  // apply to non-zero quaternion
    Quaternion UnitInverse () const;  // apply to unit-length quaternion
    Quaternion Exp () const;
    Quaternion Log () const;

    // rotation of a vector by a quaternion
    Vector3 operator* (const Vector3& rkVector) const;

    // spherical linear interpolation
    static Quaternion Slerp (float fT, const Quaternion& rkP,
        const Quaternion& rkQ);

    static Quaternion SlerpExtraSpins (float fT,
        const Quaternion& rkP, const Quaternion& rkQ,
        int iExtraSpins);

    // setup for spherical quadratic interpolation
    static void Intermediate (const Quaternion& rkQ0,
        const Quaternion& rkQ1, const Quaternion& rkQ2,
        Quaternion& rka, Quaternion& rkB);

    // spherical quadratic interpolation
    static Quaternion Squad (float fT, const Quaternion& rkP,
        const Quaternion& rkA, const Quaternion& rkB,
        const Quaternion& rkQ);

    // cutoff for sine near zero
    static const float ms_fEpsilon;

    // special values
    static const Quaternion ZERO;
    static const Quaternion IDENTITY;

    float w, x, y, z;

    /** Function for writing to a stream.
    */
    inline friend std::ostream& operator <<
        ( std::ostream& o, const Quaternion& q )
    {
        o << "[" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << "]";
        return o;
    }

// ajout de jeremie GRAULLE

    Quaternion (const float& rfAngle, const Vector3& rkAxis);

};


#endif
