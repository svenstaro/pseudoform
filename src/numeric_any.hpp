 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef NUMERIC_ANY_HPP_INCLUDED
#define NUMERIC_ANY_HPP_INCLUDED

#include <boost/any.hpp>

namespace engine
{

    template<typename T = float>
    T numeric_any(const boost::any &a)
    {
        // try different numerics
        try { T t = boost::any_cast<T>(a); return t; } catch (...) { }
        try { float t = boost::any_cast<float>(a); return t; } catch (...) { }
        try { double t = boost::any_cast<double>(a); return t; } catch (...) { }
        try { signed t = boost::any_cast<signed>(a); return t; } catch (...) { }
        try { unsigned t = boost::any_cast<unsigned>(a); return t; } catch (...) { }

        return T(0);
    }


}

#endif // NUMERIC_ANY_HPP_INCLUDED
