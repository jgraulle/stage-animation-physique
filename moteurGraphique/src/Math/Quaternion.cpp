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

#include "Quaternion.h"

#include <math.h>
#include "Matrix3.h"
#include "Vector3.h"


const float Quaternion::ms_fEpsilon = 1e-03;
const Quaternion Quaternion::ZERO(0.0,0.0,0.0,0.0);
const Quaternion Quaternion::IDENTITY(1.0,0.0,0.0,0.0);

//-----------------------------------------------------------------------
Quaternion::Quaternion (float fW, float fX, float fY, float fZ)
{
    w = fW;
    x = fX;
    y = fY;
    z = fZ;
}
//-----------------------------------------------------------------------
Quaternion::Quaternion (const Quaternion& rkQ)
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;
}
//-----------------------------------------------------------------------
void Quaternion::FromRotationMatrix (const Matrix3& kRot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    float fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
    float fRoot;

    if ( fTrace > 0.0 )
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = sqrt(fTrace + 1.0);  // 2w
        w = 0.5*fRoot;
        fRoot = 0.5/fRoot;  // 1/(4w)
        x = (kRot[2][1]-kRot[1][2])*fRoot;
        y = (kRot[0][2]-kRot[2][0])*fRoot;
        z = (kRot[1][0]-kRot[0][1])*fRoot;
    }
    else
    {
        // |w| <= 1/2
        static int s_iNext[3] = { 1, 2, 0 };
        int i = 0;
        if ( kRot[1][1] > kRot[0][0] )
            i = 1;
        if ( kRot[2][2] > kRot[i][i] )
            i = 2;
        int j = s_iNext[i];
        int k = s_iNext[j];

        fRoot = sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0);
        float* apkQuat[3] = { &x, &y, &z };
        *apkQuat[i] = 0.5*fRoot;
        fRoot = 0.5/fRoot;
        w = (kRot[k][j]-kRot[j][k])*fRoot;
        *apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
        *apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
    }
}
//-----------------------------------------------------------------------
void Quaternion::ToRotationMatrix (Matrix3& kRot) const
{
    float fTx  = 2.0*x;
    float fTy  = 2.0*y;
    float fTz  = 2.0*z;
    float fTwx = fTx*w;
    float fTwy = fTy*w;
    float fTwz = fTz*w;
    float fTxx = fTx*x;
    float fTxy = fTy*x;
    float fTxz = fTz*x;
    float fTyy = fTy*y;
    float fTyz = fTz*y;
    float fTzz = fTz*z;

    kRot[0][0] = 1.0-(fTyy+fTzz);
    kRot[0][1] = fTxy-fTwz;
    kRot[0][2] = fTxz+fTwy;
    kRot[1][0] = fTxy+fTwz;
    kRot[1][1] = 1.0-(fTxx+fTzz);
    kRot[1][2] = fTyz-fTwx;
    kRot[2][0] = fTxz-fTwy;
    kRot[2][1] = fTyz+fTwx;
    kRot[2][2] = 1.0-(fTxx+fTyy);
}
//-----------------------------------------------------------------------
void Quaternion::FromAngleAxis (const float& rfAngle,
    const Vector3& rkAxis)
{
    // assert:  axis[] is unit length
    //
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    float fHalfAngle = 0.5*rfAngle;
    float fSin = sin(fHalfAngle);
    w = cos(fHalfAngle);
    x = fSin*rkAxis.x;
    y = fSin*rkAxis.y;
    z = fSin*rkAxis.z;
}
//-----------------------------------------------------------------------
void Quaternion::ToAngleAxis (float& rfAngle, Vector3& rkAxis) const
{
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    float fSqrLength = x*x+y*y+z*z;
    if ( fSqrLength > 0.0 )
    {
        rfAngle = 2.0*acos(w);
        float fInvLength = 1.0/sqrt(fSqrLength);
        rkAxis.x = x*fInvLength;
        rkAxis.y = y*fInvLength;
        rkAxis.z = z*fInvLength;
    }
    else
    {
        // angle is 0 (mod 2*pi), so any axis will do
        rfAngle = 0.0;
        rkAxis.x = 1.0;
        rkAxis.y = 0.0;
        rkAxis.z = 0.0;
    }
}
//-----------------------------------------------------------------------
void Quaternion::FromAxes (const Vector3* akAxis)
{
    Matrix3 kRot;

    for (int iCol = 0; iCol < 3; iCol++)
    {
        kRot[0][iCol] = akAxis[iCol].x;
        kRot[1][iCol] = akAxis[iCol].y;
        kRot[2][iCol] = akAxis[iCol].z;
    }

    FromRotationMatrix(kRot);
}
//-----------------------------------------------------------------------
void Quaternion::FromAxes (const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
{
    Matrix3 kRot;

    kRot[0][0] = xAxis.x;
    kRot[1][0] = xAxis.y;
    kRot[2][0] = xAxis.z;

    kRot[0][1] = yAxis.x;
    kRot[1][1] = yAxis.y;
    kRot[2][1] = yAxis.z;

    kRot[0][2] = zAxis.x;
    kRot[1][2] = zAxis.y;
    kRot[2][2] = zAxis.z;

    FromRotationMatrix(kRot);

}
//-----------------------------------------------------------------------
void Quaternion::ToAxes (Vector3* akAxis) const
{
    Matrix3 kRot;

    ToRotationMatrix(kRot);

    for (int iCol = 0; iCol < 3; iCol++)
    {
        akAxis[iCol].x = kRot[0][iCol];
        akAxis[iCol].y = kRot[1][iCol];
        akAxis[iCol].z = kRot[2][iCol];
    }
}
//-----------------------------------------------------------------------
void Quaternion::ToAxes (Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const
{
    Matrix3 kRot;

    ToRotationMatrix(kRot);

    xAxis.x = kRot[0][0];
    xAxis.y = kRot[1][0];
    xAxis.z = kRot[2][0];

    yAxis.x = kRot[0][1];
    yAxis.y = kRot[1][1];
    yAxis.z = kRot[2][1];

    zAxis.x = kRot[0][2];
    zAxis.y = kRot[1][2];
    zAxis.z = kRot[2][2];
}

//-----------------------------------------------------------------------
Quaternion& Quaternion::operator= (const Quaternion& rkQ)
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;
    return *this;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator+ (const Quaternion& rkQ) const
{
    return Quaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator- (const Quaternion& rkQ) const
{
    return Quaternion(w-rkQ.w,x-rkQ.x,y-rkQ.y,z-rkQ.z);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator* (const Quaternion& rkQ) const
{
    // NOTE:  Multiplication is not generally commutative, so in most
    // cases p*q != q*p.

    return Quaternion
    (
        w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
        w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
        w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
        w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
    );
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator* (float fScalar) const
{
    return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
}
//-----------------------------------------------------------------------
Quaternion operator* (float fScalar, const Quaternion& rkQ)
{
    return Quaternion(fScalar*rkQ.w,fScalar*rkQ.x,fScalar*rkQ.y,
        fScalar*rkQ.z);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator- () const
{
    return Quaternion(-w,-x,-y,-z);
}
//-----------------------------------------------------------------------
float Quaternion::Dot (const Quaternion& rkQ) const
{
    return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
}
//-----------------------------------------------------------------------
float Quaternion::Norm () const
{
    return w*w+x*x+y*y+z*z;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::Inverse () const
{
    float fNorm = w*w+x*x+y*y+z*z;
    if ( fNorm > 0.0 )
    {
        float fInvNorm = 1.0/fNorm;
        return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
    }
    else
    {
        // return an invalid result to flag the error
        return ZERO;
    }
}
//-----------------------------------------------------------------------
Quaternion Quaternion::UnitInverse () const
{
    // assert:  'this' is unit length
    return Quaternion(w,-x,-y,-z);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::Exp () const
{
    // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

    float fAngle = sqrt(x*x+y*y+z*z);
    float fSin = sin(fAngle);

    Quaternion kResult;
    kResult.w = cos(fAngle);

    if ( fabs(fSin) >= ms_fEpsilon )
    {
        float fCoeff = fSin/fAngle;
        kResult.x = fCoeff*x;
        kResult.y = fCoeff*y;
        kResult.z = fCoeff*z;
    }
    else
    {
        kResult.x = x;
        kResult.y = y;
        kResult.z = z;
    }

    return kResult;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::Log () const
{
    // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
    // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

    Quaternion kResult;
    kResult.w = 0.0;

    if ( fabs(w) < 1.0 )
    {
        float fAngle = acos(w);
        float fSin = sin(fAngle);
        if ( fabs(fSin) >= ms_fEpsilon )
        {
            float fCoeff = fAngle/fSin;
            kResult.x = fCoeff*x;
            kResult.y = fCoeff*y;
            kResult.z = fCoeff*z;
            return kResult;
        }
    }

    kResult.x = x;
    kResult.y = y;
    kResult.z = z;

    return kResult;
}
//-----------------------------------------------------------------------
Vector3 Quaternion::operator* (const Vector3& v) const
{
    // nVidia SDK implementation
    Vector3 uv, uuv;
    Vector3 qvec(x, y, z);
    uv = qvec.crossProduct(v);
    uuv = qvec.crossProduct(uv);
    uv *= (2.0f * w);
    uuv *= 2.0f;

    return v + uv + uuv;

}
//-----------------------------------------------------------------------
Quaternion Quaternion::Slerp (float fT, const Quaternion& rkP,
    const Quaternion& rkQ)
{
    float fCos = rkP.Dot(rkQ);
    float fAngle = acos(fCos);

    if ( fabs(fAngle) < ms_fEpsilon )
        return rkP;

    float fSin = sin(fAngle);
    float fInvSin = 1.0/fSin;
    float fCoeff0 = sin((1.0-fT)*fAngle)*fInvSin;
    float fCoeff1 = sin(fT*fAngle)*fInvSin;
    return fCoeff0*rkP + fCoeff1*rkQ;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::SlerpExtraSpins (float fT,
    const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
{
    float fCos = rkP.Dot(rkQ);
    float fAngle = acos(fCos);

    if ( fabs(fAngle) < ms_fEpsilon )
        return rkP;

    float fSin = sin(fAngle);
    float fPhase = M_PI*iExtraSpins*fT;
    float fInvSin = 1.0/fSin;
    float fCoeff0 = sin((1.0-fT)*fAngle - fPhase)*fInvSin;
    float fCoeff1 = sin(fT*fAngle + fPhase)*fInvSin;
    return fCoeff0*rkP + fCoeff1*rkQ;
}
//-----------------------------------------------------------------------
void Quaternion::Intermediate (const Quaternion& rkQ0,
    const Quaternion& rkQ1, const Quaternion& rkQ2,
    Quaternion& rkA, Quaternion& rkB)
{
    // assert:  q0, q1, q2 are unit quaternions

    Quaternion kQ0inv = rkQ0.UnitInverse();
    Quaternion kQ1inv = rkQ1.UnitInverse();
    Quaternion rkP0 = kQ0inv*rkQ1;
    Quaternion rkP1 = kQ1inv*rkQ2;
    Quaternion kArg = 0.25*(rkP0.Log()-rkP1.Log());
    Quaternion kMinusArg = -kArg;

    rkA = rkQ1*kArg.Exp();
    rkB = rkQ1*kMinusArg.Exp();
}
//-----------------------------------------------------------------------
Quaternion Quaternion::Squad (float fT,
    const Quaternion& rkP, const Quaternion& rkA,
    const Quaternion& rkB, const Quaternion& rkQ)
{
    float fSlerpT = 2.0*fT*(1.0-fT);
    Quaternion kSlerpP = Slerp(fT,rkP,rkQ);
    Quaternion kSlerpQ = Slerp(fT,rkA,rkB);
    return Slerp(fSlerpT,kSlerpP,kSlerpQ);
}
//-----------------------------------------------------------------------
bool Quaternion::operator== (const Quaternion& rhs) const
{
    return (rhs.x == x) && (rhs.y == y) &&
        (rhs.z == z) && (rhs.w == w);
}


// ajout de jeremie GRAULLE

Quaternion::Quaternion (const float& rfAngle, const Vector3& rkAxis) {
	FromAngleAxis(rfAngle, rkAxis);
}
