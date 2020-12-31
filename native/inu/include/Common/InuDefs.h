/*
 * File - InuDefs.h
 *
 * This file is part of the Inuitive SDK
 *
 * Copyright (C) 2014 All rights reserved to Inuitive  
 *
 */

#ifndef __INUDEFS_H__
#define __INUDEFS_H__

//#include "StreamsExport.h"
//#include "InuError.h"

#include <cassert>
#include <limits>
#include <complex>      // std::abs
#include <iostream>

namespace InuDev
{

    /// \brief    Byte defintion (used for binary data buffers)
    typedef unsigned char byte;

    template <class T> struct DefaultLessFunctor;
    ////////////////////////////////////////////////////////////////////////
    /// \brief    General representation of Point.
    ///
    /// Role: Represents point of any dimension and of any type.
    ///
    /// Responsibilities:
    ///     1. Knows how to create Point from different input data.
    ///     2. Access to all coordinates.
    ///     3. Arithmetic operators
    ///
    /// Comments: Template parameters:
    ///     Dim - Point dimension
    ///     Type - Value type
    ///     LessFunctor - Provides ordering relation
    ///
    ////////////////////////////////////////////////////////////////////////
    template<int Dim, class Type, class LessFunctor = InuDev::DefaultLessFunctor < Type >>
    class CPoint
    {
    public:

        /// \brief    Empty constructor
        CPoint<Dim, Type, LessFunctor>()
        {
            for (size_t i = 0; i < Dim; i++)
            {
                mElements[i] = Type();
            }
        }

        /// \brief    2 Dimensions constructor
        CPoint<Dim, Type, LessFunctor>(Type x, Type y)
        {
            assert(Dim > 1);
            mElements[0] = x;
            mElements[1] = y;
        }

        /// \brief    3 Dimensions constructor
        CPoint<Dim, Type, LessFunctor>(float x, float y, float z)
        {
            assert(Dim > 2);
            mElements[0] = x;
            mElements[1] = y;
            mElements[2] = z;
        }

        /// \brief    4 Dimensions constructor
        CPoint<Dim, Type, LessFunctor>(float x, float y, float z, float w)
        {
            assert(Dim > 3);
            mElements[0] = x;
            mElements[1] = y;
            mElements[2] = z;
            mElements[3] = w;
        }

        Type X() const
        {
            return operator[](0);
        }
        Type& X()
        {
            return operator[](0);
        }

        Type Y() const
        {
            return operator[](1);
        }
        Type& Y()
        {
            return operator[](1);
        }

        Type Z() const
        {
            return operator[](2);
        }
        Type& Z()
        {
            return operator[](2);
        }

        Type W() const
        {
            return operator[](3);
        }
        Type& W()
        {
            return operator[](3);
        }

        Type operator[](size_t index) const
        {
            assert(Dim > index);
            return mElements[index];
        }

        Type& operator[](size_t index)
        {
            assert(Dim > index);
            return mElements[index];
        }

        bool operator==(const CPoint<Dim, Type, LessFunctor>& input) const
        {
            for (int i = 0; i < Dim; i++)
            {
                if (LessFunctor()(mElements[i], input.mElements[i])
                    || LessFunctor()(input.mElements[i], mElements[i]))
                {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const CPoint<Dim, Type, LessFunctor>& input) const
        {
            return !(*this == input);
        }

        bool operator<(const CPoint<Dim, Type, LessFunctor>& input) const
        {
            for (int i = 0; i < Dim; i++)
            {
                if (LessFunctor()(input.mElements[i], mElements[i]))
                {
                    return false;
                }
            }
            if (*this == input)
            {
                return false;
            }
            return true;
        }

        void operator+=(const CPoint<Dim, Type, LessFunctor>& input)
        {
            for (int i = 0; i < Dim; i++)
            {
                mElements[i] += input.mElements[i];
            }
        }

        CPoint<Dim, Type, LessFunctor> operator+(
            const CPoint<Dim, Type, LessFunctor>& input) const
        {
            CPoint<Dim, Type, LessFunctor> result(*this);
            result += input;
            return result;
        }

        void operator-=(const CPoint<Dim, Type, LessFunctor>& input)
        {
            for (int i = 0; i < Dim; i++)
            {
                mElements[i] -= input.mElements[i];
            }
        }

        CPoint<Dim, Type, LessFunctor> operator-(
            const CPoint<Dim, Type, LessFunctor>& input) const
        {
            CPoint<Dim, Type, LessFunctor> result(*this);
            result -= input;
            return result;
        }

        void operator*=(const CPoint<Dim, Type, LessFunctor>& input)
        {
            for (int i = 0; i < Dim; i++)
            {
                mElements[i] *= input.mElements[i];
            }
        }

        CPoint<Dim, Type, LessFunctor> operator*(
            const CPoint<Dim, Type, LessFunctor>& input) const
        {
            CPoint<Dim, Type, LessFunctor> result(*this);
            result *= input;
            return result;
        }

        void operator/=(const CPoint<Dim, Type, LessFunctor>& input)
        {
            for (int i = 0; i < Dim; i++)
            {
                mElements[i] /= input.mElements[i];
            }
        }

        CPoint<Dim, Type, LessFunctor> operator/(
            const CPoint<Dim, Type, LessFunctor>& input) const
        {
            CPoint<Dim, Type, LessFunctor> result(*this);
            result /= input;
            return result;
        }

        CPoint<Dim, Type, LessFunctor> absolute() const
        {
            CPoint<Dim, Type, LessFunctor> result;
            for (int i = 0; i < Dim; i++)
            {
                result.mElements[i] = std::abs(mElements[i]);
            }
            return result;
        }

        /// \brief    Square distance
        Type SqrDist(const CPoint<Dim, Type, LessFunctor>& input) const
        {
            assert(Dim > 0);
            Type result = (mElements[0] - input.mElements[0])
                * (mElements[0] - input.mElements[0]);
            for (int i = 1; i < Dim; i++)
            {
                result += (mElements[i] - input.mElements[i])
                    * (mElements[i] - input.mElements[i]);
            }
            return result;
        }

        const Type* GetElements() const { return mElements; }

        friend std::ostream & operator << (std::ostream &out, const CPoint<Dim, Type, LessFunctor> &input)
        {
            for (const auto& elem : input.mElements)
            {
                out << elem << " ";
            }
            return out;
        }

        friend std::istream & operator >> (std::istream &in, const CPoint<Dim, Type, LessFunctor> &output)
        {
            for (const auto& elem : output.mElements)
            {
                in << elem;
            }
            return in;
        }


protected:

    /// \brief    value of each coordinate
    Type mElements[Dim];
};

////////////////////////////////////////////////////////////////////////
/// \brief    Ordering relation functor for float CPoint
////////////////////////////////////////////////////////////////////////
struct LessFunctorFloat
{
    bool operator()(float first, float second)
    {
        return (first < second - std::numeric_limits<float>::epsilon());
    }
};

struct LessFunctorDouble
{
    bool operator()(float first, float second)
    {
        return (first < second - std::numeric_limits<double>::epsilon());
    }
};

template <class T>
struct DefaultLessFunctor
{
    bool operator()(T first, T second)
    {
        return (first < second);
    }
};

/// \brief    2 Dimensional float point prototype
typedef CPoint<2, float, LessFunctorFloat> CPoint2D;

/// \brief    3 Dimensional float point prototype
typedef CPoint<3, float, LessFunctorFloat> CPoint3D;

/// \brief    4 Dimensional float point prototype
typedef CPoint<4, float, LessFunctorFloat> CPoint4D;
}

#endif //__INUDEFS_H__
