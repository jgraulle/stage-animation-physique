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
#ifndef __Matrix4__
#define __Matrix4__

#include "Vector3.h"
#include "Matrix3.h"

/** Class encapsulating a standard 4x4 homogenous matrix.
    @remarks
        OGRE uses column vectors when applying matrix multiplications,
        This means a vector is represented as a single column, 4-row
        matrix. This has the effect that the tranformations implemented
        by the matrices happens right-to-left e.g. if vector V is to be
        transformed by M1 then M2 then M3, the calculation would be
        M3 * M2 * M1 * V. The order that matrices are concatenated is
        vital since matrix multiplication is not cummatative, i.e. you
        can get a different result if you concatenate in the wrong order.
    @par
        The use of column vectors and right-to-left ordering is the
        standard in most mathematical texts, and id the same as used in
        OpenGL. It is, however, the opposite of Direct3D, which has
        inexplicably chosen to differ from the accepted standard and uses
        row vectors and left-to-right matrix multiplication.
    @par
        OGRE deals with the differences between D3D and OpenGL etc.
        internally when operating through different render systems. OGRE
        users only need to conform to standard maths conventions, i.e.
        right-to-left matrix multiplication, (OGRE transposes matrices it
        passes to D3D to compensate).
    @par
        The generic form M * V which shows the layout of the matrix
        entries is shown below:
        <pre>
            [ m[0][0]  m[0][1]  m[0][2]  m[0][3] ]   {x}
            | m[1][0]  m[1][1]  m[1][2]  m[1][3] | * {y}
            | m[2][0]  m[2][1]  m[2][2]  m[2][3] |   {z}
            [ m[3][0]  m[3][1]  m[3][2]  m[3][3] ]   {1}
        </pre>
*/
class Matrix4
{
protected:
    /// The matrix entries, indexed by [row][col].
    union {
        float m[4][4];
        float _m[16];
    };
public:
    /** Default constructor.
        @note
            It does <b>NOT</b> initialize the matrix for efficiency.
    */
    inline Matrix4()
    {
    }

    inline Matrix4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33 )
    {
        m[0][0] = m00;
        m[0][1] = m01;
        m[0][2] = m02;
        m[0][3] = m03;
        m[1][0] = m10;
        m[1][1] = m11;
        m[1][2] = m12;
        m[1][3] = m13;
        m[2][0] = m20;
        m[2][1] = m21;
        m[2][2] = m22;
        m[2][3] = m23;
        m[3][0] = m30;
        m[3][1] = m31;
        m[3][2] = m32;
        m[3][3] = m33;
    }
/*
    inline operator float* (void)
    {
        return _m;
    }

    inline operator const float* (void)
    {
        return _m;
    }
*/
    inline const float* getData()
    {
        return _m;
    }

    inline float* operator [] ( unsigned iRow )
    {
        assert( iRow < 4 );
        return m[iRow];
    }

    inline const float *const operator [] ( unsigned iRow ) const
    {
        assert( iRow < 4 );
        return m[iRow];
    }

    inline Matrix4 concatenate(const Matrix4 &m2) const
    {
        Matrix4 r;
        r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
        r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
        r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
        r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

        r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
        r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
        r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
        r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

        r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
        r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
        r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
        r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

        r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
        r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
        r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
        r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

        return r;
    }

    /** Matrix concatenation using '*'.
    */
    inline Matrix4 operator * ( const Matrix4 &m2 ) const
    {
        return concatenate( m2 );
    }

    /** Vector transformation using '*'.
        @remarks
            Transforms the given 3-D vector by the matrix, projecting the
            result back into <i>w</i> = 1.
        @note
            This means that the initial <i>w</i> is considered to be 1.0,
            and then all the tree elements of the resulting 3-D vector are
            divided by the resulting <i>w</i>.
    */
    inline Vector3 operator * ( const Vector3 &v ) const
    {
        Vector3 r;

        float fInvW = 1.0 / ( m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] );

        r.x = ( m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] ) * fInvW;
        r.y = ( m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] ) * fInvW;
        r.z = ( m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] ) * fInvW;

        return r;
    }

    /** Matrix addition.
    */
    inline Matrix4 operator + ( const Matrix4 &m2 ) const
    {
        Matrix4 r;

        r.m[0][0] = m[0][0] + m2.m[0][0];
        r.m[0][1] = m[0][1] + m2.m[0][1];
        r.m[0][2] = m[0][2] + m2.m[0][2];
        r.m[0][3] = m[0][3] + m2.m[0][3];

        r.m[1][0] = m[1][0] + m2.m[1][0];
        r.m[1][1] = m[1][1] + m2.m[1][1];
        r.m[1][2] = m[1][2] + m2.m[1][2];
        r.m[1][3] = m[1][3] + m2.m[1][3];

        r.m[2][0] = m[2][0] + m2.m[2][0];
        r.m[2][1] = m[2][1] + m2.m[2][1];
        r.m[2][2] = m[2][2] + m2.m[2][2];
        r.m[2][3] = m[2][3] + m2.m[2][3];

        r.m[3][0] = m[3][0] + m2.m[3][0];
        r.m[3][1] = m[3][1] + m2.m[3][1];
        r.m[3][2] = m[3][2] + m2.m[3][2];
        r.m[3][3] = m[3][3] + m2.m[3][3];

        return r;
    }

    /** Matrix subtraction.
    */
    inline Matrix4 operator - ( const Matrix4 &m2 ) const
    {
        Matrix4 r;
        r.m[0][0] = m[0][0] - m2.m[0][0];
        r.m[0][1] = m[0][1] - m2.m[0][1];
        r.m[0][2] = m[0][2] - m2.m[0][2];
        r.m[0][3] = m[0][3] - m2.m[0][3];

        r.m[1][0] = m[1][0] - m2.m[1][0];
        r.m[1][1] = m[1][1] - m2.m[1][1];
        r.m[1][2] = m[1][2] - m2.m[1][2];
        r.m[1][3] = m[1][3] - m2.m[1][3];

        r.m[2][0] = m[2][0] - m2.m[2][0];
        r.m[2][1] = m[2][1] - m2.m[2][1];
        r.m[2][2] = m[2][2] - m2.m[2][2];
        r.m[2][3] = m[2][3] - m2.m[2][3];

        r.m[3][0] = m[3][0] - m2.m[3][0];
        r.m[3][1] = m[3][1] - m2.m[3][1];
        r.m[3][2] = m[3][2] - m2.m[3][2];
        r.m[3][3] = m[3][3] - m2.m[3][3];

        return r;
    }

    /** Tests 2 matrices for equality.
    */
    inline bool operator == ( const Matrix4& m2 ) const
    {
        if(
            m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
            m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
            m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
            m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
            return false;
        return true;
    }

    /** Tests 2 matrices for inequality.
    */
    inline bool operator != ( Matrix4& m2 ) const
    {
        if(
            m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
            m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
            m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
            m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
            return true;
        return false;
    }

    /** Assignment from 3x3 matrix.
    */
    inline void operator = ( const Matrix3& mat3 )
    {
        m[0][0] = mat3.m[0][0]; m[0][1] = mat3.m[0][1]; m[0][2] = mat3.m[0][2];
        m[1][0] = mat3.m[1][0]; m[1][1] = mat3.m[1][1]; m[1][2] = mat3.m[1][2];
        m[2][0] = mat3.m[2][0]; m[2][1] = mat3.m[2][1]; m[2][2] = mat3.m[2][2];
    }

    inline Matrix4 transpose(void) const
    {
        return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0],
                    m[0][1], m[1][1], m[2][1], m[3][1],
                    m[0][2], m[1][2], m[2][2], m[3][2],
                    m[0][3], m[1][3], m[2][3], m[3][3]);
    }

    /*
    -----------------------------------------------------------------------
    Translation Transformation
    -----------------------------------------------------------------------
    */
    /** Sets the translation transformation part of the matrix.
    */
    inline void setTrans( const Vector3& v )
    {
        m[0][3] = v.x;
        m[1][3] = v.y;
        m[2][3] = v.z;
    }

    /** Builds a translation matrix
    */
    inline void makeTrans( const Vector3& v )
    {
        m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = v.x;
        m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = v.y;
        m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = v.z;
        m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
    }

    inline void makeTrans( float tx, float ty, float tz )
    {
        m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = tx;
        m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = ty;
        m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = tz;
        m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
    }

    /** Gets a translation matrix.
    */
    inline static Matrix4 getTrans( const Vector3& v )
    {
        Matrix4 r;

        r.m[0][0] = 1.0; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = v.x;
        r.m[1][0] = 0.0; r.m[1][1] = 1.0; r.m[1][2] = 0.0; r.m[1][3] = v.y;
        r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = 1.0; r.m[2][3] = v.z;
        r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

        return r;
    }

    /** Gets a translation matrix - variation for not using a vector.
    */
    inline static Matrix4 getTrans( float t_x, float t_y, float t_z )
    {
        Matrix4 r;

        r.m[0][0] = 1.0; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = t_x;
        r.m[1][0] = 0.0; r.m[1][1] = 1.0; r.m[1][2] = 0.0; r.m[1][3] = t_y;
        r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = 1.0; r.m[2][3] = t_z;
        r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

        return r;
    }

    /*
    -----------------------------------------------------------------------
    Scale Transformation
    -----------------------------------------------------------------------
    */
    /** Sets the scale part of the matrix.
    */
    inline void setScale( const Vector3& v )
    {
        m[0][0] = v.x;
        m[1][1] = v.y;
        m[2][2] = v.z;
    }

    /** Gets a scale matrix.
    */
    inline static Matrix4 getScale( const Vector3& v )
    {
        Matrix4 r;
        r.m[0][0] = v.x; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = 0.0;
        r.m[1][0] = 0.0; r.m[1][1] = v.y; r.m[1][2] = 0.0; r.m[1][3] = 0.0;
        r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = v.z; r.m[2][3] = 0.0;
        r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

        return r;
    }

    /** Gets a scale matrix - variation for not using a vector.
    */
    inline static Matrix4 getScale( float s_x, float s_y, float s_z )
    {
        Matrix4 r;
        r.m[0][0] = s_x; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = 0.0;
        r.m[1][0] = 0.0; r.m[1][1] = s_y; r.m[1][2] = 0.0; r.m[1][3] = 0.0;
        r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = s_z; r.m[2][3] = 0.0;
        r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

        return r;
    }

    /** Extracts the rotation / scaling part of the Matrix as a 3x3 matrix.
    @param m3x3 Destination Matrix3
    */
    inline void extract3x3Matrix(Matrix3& m3x3) const
    {
        m3x3.m[0][0] = m[0][0];
        m3x3.m[0][1] = m[0][1];
        m3x3.m[0][2] = m[0][2];
        m3x3.m[1][0] = m[1][0];
        m3x3.m[1][1] = m[1][1];
        m3x3.m[1][2] = m[1][2];
        m3x3.m[2][0] = m[2][0];
        m3x3.m[2][1] = m[2][1];
        m3x3.m[2][2] = m[2][2];

    }

    static const Matrix4 ZERO;
    static const Matrix4 IDENTITY;

    inline Matrix4 operator*(float scalar)
    {
        return Matrix4(
            scalar*m[0][0], scalar*m[0][1], scalar*m[0][2], scalar*m[0][3],
            scalar*m[1][0], scalar*m[1][1], scalar*m[1][2], scalar*m[1][3],
            scalar*m[2][0], scalar*m[2][1], scalar*m[2][2], scalar*m[2][3],
            scalar*m[3][0], scalar*m[3][1], scalar*m[3][2], scalar*m[3][3]);
    }

    /** Function for writing to a stream.
    */
    inline friend std::ostream& operator <<
        ( std::ostream& o, const Matrix4& m )
    {
        o << "Matrix4(";
        for (int i = 0; i < 4; ++i)
        {
            o << " row" << i << "{";
            for(int j = 0; j < 4; ++j)
            {
                o << m[i][j] << " ";
            }
            o << "}";
        }
        o << ")";
        return o;
    }

    Matrix4 adjoint() const;
    float determinant() const;
    Matrix4 inverse() const;

};

#endif
