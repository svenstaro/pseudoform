 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SAFE_ANY_HPP_INCLUDED
#define SAFE_ANY_HPP_INCLUDED

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include "log.hpp"
#include "string.hpp"

/**
 * @brief Casting from one type to another one
 * @param type what to convert in
 * @param a convertable object
 *
 * @return a object converted into T type (if it's possible)
 */
template<typename T>
T safeAnyCast(const boost::any &a, const engine::string &type,
    const engine::string &file, const engine::string &func, unsigned line, const T &ret = T())
{
	// Try to cast given in second argument object
	// into the type given in first argument
    const T *t = boost::any_cast<T>(&a);
    if (!t)
    {
        engine::log("Failed any_cast<" + type + ">");
        engine::log("(" + file + " : " + func + " : " + boost::lexical_cast<engine::string>(line) + ")");
        return ret;
    }
    return *t;
}

/**
 * @def SAFE_ANY_CAST(T, a)
 * @brief This macro provides casting a-object into T type
 * @see safeAnyCast()
 */
#define SAFE_ANY_CAST(T, a) safeAnyCast<T>(a, #T, __FILE__, __FUNCTION__, __LINE__)
#define SAFE_ANY_CAST_T(T, a, ret) safeAnyCast<T>(a, #T, __FILE__, __FUNCTION__, __LINE__, ret)

#endif // SAFE_ANY_HPP_INCLUDED
