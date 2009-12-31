 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <chaiscript/chaiscript.hpp>

#include "bindings.hpp"

#include "log.hpp"
#include "types.hpp"

namespace engine
{

    namespace script
    {

        rad _toRad(deg d)
        {
            return rad(d);
        }

        deg _toDeg(rad d)
        {
            return deg(d);
        }

        real _valDeg(deg d)
        {
            return d.valueDegrees();
        }

        real _valRad(rad d)
        {
            return d.valueRadians();
        }

        quat _slerp(quat &this_, quat &dest, real amount, bool shortest)
        {
            return quat::Slerp(amount, this_, dest, shortest);
        }

        quat _nlerp(quat &this_, quat &dest, real amount, bool shortest)
        {
            return quat::nlerp(amount, this_, dest, shortest);
        }

        vec3 _axis(quat &this_, size_t n)
        {
            switch(n)
            {
                case 0:
                    return this_.xAxis();
                break;
                case 1:
                    return this_.yAxis();
                break;
                case 2:
                    return this_.zAxis();
                break;
                default:
                    return vec3::ZERO;
                break;
            }

            return vec3::ZERO;
        }

        void bindMath(script &scr)
        {
            using namespace chaiscript;

//            module(L)
//            [
//                class_<deg>("deg")
//                    .def(constructor<real>())
//                    .def("rad", &_toRad)
//                    .def("val", &_valDeg)
//
//                ,class_<rad>("rad")
//                    .def(constructor<real>())
//                    .def("deg", &_toDeg)
//                    .def("val", &_valRad)
//
//                ,class_<vec4>("vec4")
//                    .def(constructor<real, real, real, real>())
//                    .def(constructor<vec4>())
//                    // members
//                    .def_readwrite("x", &vec4::x)
//                    .def_readwrite("y", &vec4::y)
//                    .def_readwrite("z", &vec4::z)
//                    .def_readwrite("w", &vec4::w)
//                    // named functions
//                    .def("dot", &vec4::dotProduct)
//                    // operators
//                    .def(self + vec4())
//                    .def(self - vec4())
//                    .def(self * vec4())
//                    .def(self / vec4())
//                    .def(self * real())
//                    .def(self / real())
//                    .def(self == vec4())
//                    .def(-self)
//                    .def(tostring(self))
//
//                ,class_<vec3>("vec3")
//                    .def(constructor<real, real, real>())
//                    .def(constructor<vec3>())
//                    // members
//                    .def_readwrite("x", &vec3::x)
//                    .def_readwrite("y", &vec3::y)
//                    .def_readwrite("z", &vec3::z)
//                    // named functions
//                    .def("len", &vec3::length)
//                    .def("sqLen", &vec3::squaredLength)
//                    .def("norm", &vec3::normalise)
//                    .def("normCopy", &vec3::normalisedCopy)
//                    .def("dot", &vec3::dotProduct)
//                    .def("cross", &vec3::crossProduct)
//                    .def("dist", &vec3::distance)
//                    .def("mid", &vec3::midPoint)
//                    .def("perp", &vec3::perpendicular)
//                    .def("reflect", &vec3::reflect)
//                    .def("isZero", &vec3::isZeroLength)
//                    // operators
//                    .def(self + vec3())
//                    .def(self - vec3())
//                    .def(self * vec3())
//                    .def(self / vec3())
//                    .def(self * real())
//                    .def(self / real())
//                    .def(self == vec3())
//                    .def(-self)
//                    .def(tostring(self))
//
//                ,class_<vec2>("vec2")
//                    .def(constructor<real, real>())
//                    .def(constructor<vec2>())
//                    // members
//                    .def_readwrite("x", &vec2::x)
//                    .def_readwrite("y", &vec2::y)
//                    // named functions
//                    .def("len", &vec2::length)
//                    .def("sqLen", &vec2::squaredLength)
//                    .def("norm", &vec2::normalise)
//                    .def("normCopy", &vec2::normalisedCopy)
//                    .def("dot", &vec2::dotProduct)
//                    .def("mid", &vec2::midPoint)
//                    .def("reflect", &vec3::reflect)
//                    .def("isZero", &vec3::isZeroLength)
//                    // operators
//                    .def(self + vec2())
//                    .def(self - vec2())
//                    .def(self * vec2())
//                    .def(self / vec2())
//                    .def(self * real())
//                    .def(self / real())
//                    .def(self == vec2())
//                    .def(-self)
//                    .def(tostring(self))
//
//                ,class_<quat>("quat")
//                    .def(constructor<real, real, real, real>())
//                    .def(constructor<deg, vec3>())
//                    .def(constructor<rad, vec3>())
//                    .def(constructor<quat>())
//                    // named functions
//                    .def("dot", &quat::Dot)
//                    .def("norm", &quat::normalise)
//                    .def("inverse", &quat::Inverse)
//                    .def("uinverse", &quat::UnitInverse)
//                    .def("slerp", &_slerp)
//                    .def("nlerp", &_nlerp)
//                    .def("roll", &quat::getRoll)
//                    .def("pitch", &quat::getPitch)
//                    .def("yaw", &quat::getYaw)
//                    .def("axis", &_axis)
//                    // members
//                    .def_readwrite("w", &quat::w)
//                    .def_readwrite("x", &quat::x)
//                    .def_readwrite("y", &quat::y)
//                    .def_readwrite("z", &quat::z)
//                    // operators
//                    .def(self * quat())
//                    .def(self + quat())
//                    .def(self * real())
//                    .def(self * vec3())
//                    .def(self == quat())
//                    .def(-self)
//                    .def(tostring(self))
//            ];
        }

    }

}
