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
#ifndef __Vector3_H__
#define __Vector3_H__

#include <iostream>
#include <math.h>
#include "Quaternion.h"
#include <assert.h>

/** Standard 3-dimensional vector.
    @remarks
        A direction in 3D space represented as distances along the 3
        orthoganal axes (x, y, z). Note that positions, directions and
        scaling factors can be represented by a vector, depending on how
        you interpret the values.
*/
class Vector3
{
public:
    float x, y, z;

public:
    inline Vector3()
    {
    }

    inline Vector3( float fX, float fY, float fZ )
        : x( fX ), y( fY ), z( fZ )
    {
    }

    inline Vector3( float afCoordinate[3] )
        : x( afCoordinate[0] ),
        y( afCoordinate[1] ),
        z( afCoordinate[2] )
    {
    }

    inline Vector3( int afCoordinate[3] )
    {
        x = (float)afCoordinate[0];
        y = (float)afCoordinate[1];
        z = (float)afCoordinate[2];
    }

    inline Vector3( const float* const r )
        : x( r[0] ), y( r[1] ), z( r[2] )
    {
    }

    inline Vector3( const Vector3& rkVector )
        : x( rkVector.x ), y( rkVector.y ), z( rkVector.z )
    {
    }

    inline void set(float fX, float fY, float fZ)
    {
        x = fX;
        y = fY;
        z = fZ;
    }

    inline operator float* ()
    {
        return &x;
    }

    inline operator const float* () const
    {
        return &x;
    }

    inline const float* data(void) const
    {
        return &x;
    }

    /** Assigns the value of the other vector.
        @param
            rkVector The other vector
    */
    inline Vector3& operator = ( const Vector3& rkVector )
    {
        x = rkVector.x;
        y = rkVector.y;
        z = rkVector.z;

        return *this;
    }

    inline bool operator == ( const Vector3& rkVector ) const
    {
        return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
    }

    inline bool operator != ( const Vector3& rkVector ) const
    {
        return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
    }

    // arithmetic operations
    inline Vector3 operator + ( const Vector3& rkVector ) const
    {
        Vector3 kSum;

        kSum.x = x + rkVector.x;
        kSum.y = y + rkVector.y;
        kSum.z = z + rkVector.z;

        return kSum;
    }

    inline Vector3 operator - ( const Vector3& rkVector ) const
    {
        Vector3 kDiff;

        kDiff.x = x - rkVector.x;
        kDiff.y = y - rkVector.y;
        kDiff.z = z - rkVector.z;

        return kDiff;
    }

    inline Vector3 operator * ( float fScalar ) const
    {
        Vector3 kProd;

        kProd.x = fScalar*x;
        kProd.y = fScalar*y;
        kProd.z = fScalar*z;

        return kProd;
    }

    inline Vector3 operator * ( const Vector3& rhs) const
    {
        Vector3 kProd;

        kProd.x = rhs.x * x;
        kProd.y = rhs.y * y;
        kProd.z = rhs.z * z;

        return kProd;
    }

    inline Vector3 operator / ( float fScalar ) const
    {
        assert( fScalar != 0.0 );

        Vector3 kDiv;

        float fInv = 1.0 / fScalar;
        kDiv.x = x * fInv;
        kDiv.y = y * fInv;
        kDiv.z = z * fInv;

        return kDiv;
    }

    inline Vector3 operator - () const
    {
        Vector3 kNeg;

        kNeg.x = -x;
        kNeg.y = -y;
        kNeg.z = -z;

        return kNeg;
    }

    inline friend Vector3 operator * ( float fScalar, const Vector3& rkVector )
    {
        Vector3 kProd;

        kProd.x = fScalar * rkVector.x;
        kProd.y = fScalar * rkVector.y;
        kProd.z = fScalar * rkVector.z;

        return kProd;
    }

    // arithmetic updates
    inline Vector3& operator += ( const Vector3& rkVector )
    {
        x += rkVector.x;
        y += rkVector.y;
        z += rkVector.z;

        return *this;
    }

    inline Vector3& operator -= ( const Vector3& rkVector )
    {
        x -= rkVector.x;
        y -= rkVector.y;
        z -= rkVector.z;

        return *this;
    }

    inline Vector3& operator *= ( float fScalar )
    {
        x *= fScalar;
        y *= fScalar;
        z *= fScalar;
        return *this;
    }

    inline Vector3& operator /= ( float fScalar )
    {
        assert( fScalar != 0.0 );

        float fInv = 1.0 / fScalar;

        x *= fInv;
        y *= fInv;
        z *= fInv;

        return *this;
    }

    /** Returns the length (magnitude) of the vector.
        @warning
            This operation requires a square root and is expensive in
            terms of CPU operations. If you don't need to know the exact
            length (e.g. for just comparing lengths) use squaredLength()
            instead.
    */
    inline float length () const
    {
        return sqrt( x * x + y * y + z * z );
    }

    /** Returns the square of the length(magnitude) of the vector.
        @remarks
            This  method is for efficiency - calculating the actual
            length of a vector requires a square root, which is expensive
            in terms of the operations required. This method returns the
            square of the length of the vector, i.e. the same as the
            length but before the square root is taken. Use this if you
            want to find the longest / shortest vector without incurring
            the square root.
    */
    inline float squaredLength () const
    {
        return x * x + y * y + z * z;
    }

    /** Calculates the dot (scalar) product of this vector with another.
        @remarks
            The dot product can be used to calculate the angle between 2
            vectors. If both are unit vectors, the dot product is the
            cosine of the angle; otherwise the dot product must be
            divided by the product of the lengths of both vectors to get
            the cosine of the angle. This result can further be used to
            calculate the distance of a point from a plane.
        @param
            vec Vector with which to calculate the dot product (together
            with this one).
        @returns
            A float representing the dot product value.
    */
    inline float dotProduct(const Vector3& vec) const
    {
        return x * vec.x + y * vec.y + z * vec.z;
    }

    /** Normalises the vector.
        @remarks
            This method normalises the vector such that it's
            length / magnitude is 1. The result is called a unit vector.
        @bug
            This function will not crash for zero-sized vectors, but there
            will be no changes made to their components.
        @returns The previous length of the vector.
    */
    inline float normalise()
    {
        float fLength = sqrt( x * x + y * y + z * z );

        // Will also work for zero-sized vectors, but will change nothing
        if ( fLength > 1e-06 )
        {
            float fInvLength = 1.0 / fLength;
            x *= fInvLength;
            y *= fInvLength;
            z *= fInvLength;
        }

        return fLength;
    }
    inline float invsqrt(float x)
    {
        float half = 0.5f*x;
        void * xAddr = &x;
        int i = *((int*)xAddr);
        i = 0x5f3759df - (i >> 1);
        void * iAddr = &i;
        x = *(float*)iAddr;
        x = x*(1.5f - half*x*x);
        return x;
    }
    inline void normalisef()
    {
        float l2 = x*x + y*y + z*z;
        float fInvLength = invsqrt(l2);
        x *= fInvLength;
        y *= fInvLength;
        z *= fInvLength;
    }
    inline void normaliseAndScalef(float s)
    {
        float l2 = x*x + y*y + z*z;
        float scale = invsqrt(l2) * s;
        x *= scale;
        y *= scale;
        z *= scale;
    }
    /** Calculates the cross-product of 2 vectors, i.e. the vector that
        lies perpendicular to them both.
        @remarks
            The cross-product is normally used to calculate the normal
            vector of a plane, by calculating the cross-product of 2
            non-equivalent vectors which lie on the plane (e.g. 2 edges
            of a triangle).
        @param
            vec Vector which, together with this one, will be used to
            calculate the cross-product.
        @returns
            A vector which is the result of the cross-product. This
            vector will <b>NOT</b> be normalised, to maximise efficiency
            - call Vector3::normalise on the result if you wish this to
            be done. As for which side the resultant vector will be on, the
            returned vector will be on the side from which the arc from 'this'
            to rkVector is anticlockwise, e.g. UNIT_Y.crossProduct(UNIT_Z)
            = UNIT_X, whilst UNIT_Z.crossProduct(UNIT_Y) = -UNIT_X.
        @par
            For a clearer explanation, look a the left and the bottom edges
            of your monitor's screen. Assume that the first vector is the
            left edge and the second vector is the bottom edge, both of
            them starting from the lower-left corner of the screen. The
            resulting vector is going to be perpendicular to both of them
            and will go <i>inside</i> the screen, towards the cathode tube
            (assuming you're using a CRT monitor, of course).
    */
    inline Vector3 crossProduct( const Vector3& rkVector ) const
    {
        Vector3 kCross;

        kCross.x = y * rkVector.z - z * rkVector.y;
        kCross.y = z * rkVector.x - x * rkVector.z;
        kCross.z = x * rkVector.y - y * rkVector.x;

        return kCross;
    }

    /** Returns a vector at a point half way between this and the passed
        in vector.
    */
    inline Vector3 midPoint( const Vector3& vec ) const
    {
        return Vector3(
            ( x + vec.x ) * 0.5,
            ( y + vec.y ) * 0.5,
            ( z + vec.z ) * 0.5 );
    }

    /** Returns true if the vector's scalar components are all greater
        that the ones of the vector it is compared against.
    */
    inline bool operator < ( const Vector3& rhs ) const
    {
        if( x < rhs.x && y < rhs.y && z < rhs.z )
            return true;
        return false;
    }

    /** Returns true if the vector's scalar components are all smaller
        that the ones of the vector it is compared against.
    */
    inline bool operator > ( const Vector3& rhs ) const
    {
        if( x > rhs.x && y > rhs.y && z > rhs.z )
            return true;
        return false;
    }

    /** Sets this vector's components to the minimum of its own and the
        ones of the passed in vector.
        @remarks
            'Minimum' in this case means the combination of the lowest
            value of x, y and z from both vectors. Lowest is taken just
            numerically, not magnitude, so -1 < 0.
    */
    inline void makeFloor( const Vector3& cmp )
    {
        if( cmp.x < x ) x = cmp.x;
        if( cmp.y < y ) y = cmp.y;
        if( cmp.z < z ) z = cmp.z;
    }

    /** Sets this vector's components to the maximum of its own and the
        ones of the passed in vector.
        @remarks
            'Maximum' in this case means the combination of the highest
            value of x, y and z from both vectors. Highest is taken just
            numerically, not magnitude, so 1 > -3.
    */
    inline void makeCeil( const Vector3& cmp )
    {
        if( cmp.x > x ) x = cmp.x;
        if( cmp.y > y ) y = cmp.y;
        if( cmp.z > z ) z = cmp.z;
    }

    /** Generates a vector perpendicular to this vector (eg an 'up' vector).
        @remarks
            This method will return a vector which is perpendicular to this
            vector. There are an infinite number of possibilities but this
            method will guarantee to generate one of them. If you need more
            control you should use the Quaternion class.
    */
    inline Vector3 perpendicular(void) const
    {
        static const float fSquareZero = 1e-06 * 1e-06;

        Vector3 perp = this->crossProduct( Vector3::UNIT_X );

        // Check length
        if( perp.squaredLength() < fSquareZero )
        {
            /* This vector is the Y axis multiplied by a scalar, so we have
            to use another axis.
            */
            perp = this->crossProduct( Vector3::UNIT_Y );
        }

        return perp;
    }
    /** Generates a new random vector which deviates from this vector by a
        given angle in a random direction.
        @remarks
            This method assumes that the random number generator has already
            been seeded appropriately.
        @param
            angle The angle at which to deviate in radians
        @param
            up Any vector perpendicular to this one (which could generated
            by cross-product of this vector and any other non-colinear
            vector). If you choose not to provide this the function will
            derive one on it's own, however if you provide one yourself the
            function will be faster (this allows you to reuse up vectors if
            you call this method more than once)
        @returns
            A random vector which deviates from this vector by angle. This
            vector will not be normalised, normalise it if you wish
            afterwards.
    */
    /*
    inline Vector3 randomDeviant(
        float angle,
        const Vector3& up = Vector3::ZERO ) const
    {
        Vector3 newUp;

        if (up == Vector3::ZERO)
        {
            // Generate an up vector
            newUp = this->perpendicular();
        }
        else
        {
            newUp = up;
        }

        // Rotate up vector by random amount around this
        Quaternion q;
        q.FromAngleAxis( random() * 2.*M_PI, *this );
        newUp = q * newUp;

        // Finally rotate this by given angle around randomised up
        q.FromAngleAxis( angle, newUp );
        return q * (*this);
    }
    */

    /** Gets the shortest arc quaternion to rotate this vector to the destination
        vector.
    @remarks
        Don't call this if you think the dest vector can be close to the inverse
        of this vector, since then ANY axis of rotation is ok.
    */
    Quaternion getRotationTo(const Vector3& dest) const
    {
        // Based on Stan Melax's article in Game Programming Gems
        Quaternion q;
        // Copy, since cannot modify local
        Vector3 v0 = *this;
        Vector3 v1 = dest;
        v0.normalise();
        v1.normalise();

        Vector3 c = v0.crossProduct(v1);

        // NB if the crossProduct approaches zero, we get unstable because ANY axis will do
        // when v0 == -v1
        float d = v0.dotProduct(v1);
        // If dot == 1, vectors are the same
        if (d >= 1.0f)
        {
            return Quaternion::IDENTITY;
        }
        float s = sqrt( (1+d)*2 );
        float invs = 1 / s;


        q.x = c.x * invs;
        q.y = c.y * invs;
        q.z = c.z * invs;
        q.w = s * 0.5;
        return q;
    }

    /** Returns true if this vector is zero length. */
    inline bool isZeroLength(void) const
    {
        float sqlen = (x * x) + (y * y) + (z * z);
        return (sqlen < (1e-06 * 1e-06));

    }

    /** As normalise, except that this vector is unaffected and the
        normalised vector is returned as a copy. */
    inline Vector3 normalisedCopy(void) const
    {
        Vector3 ret = *this;
        ret.normalise();
        return ret;
    }

    /** Calculates a reflection vector to the plane with the given normal .
    @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
    */
    inline Vector3 reflect(const Vector3& normal)
    {
        return Vector3( ( 2 * this->dotProduct(normal) * normal ) - *this );
    }

    // special points
    static const Vector3 ZERO;
    static const Vector3 UNIT_X;
    static const Vector3 UNIT_Y;
    static const Vector3 UNIT_Z;
    static const Vector3 UNIT_SCALE;

    /** Function for writing to a stream.
    */
    inline friend std::ostream& operator <<
        ( std::ostream& o, const Vector3& v )
    {
        o << "[" << v.x << ", " << v.y << ", " << v.z << "]";
        return o;
    }
};

#endif
