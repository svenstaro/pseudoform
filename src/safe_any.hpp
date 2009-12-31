 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SAFE_ANY_HPP_INCLUDED
#define SAFE_ANY_HPP_INCLUDED

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include "log.hpp"
#include "string.hpp"

template<typename T>
T safeAnyCast(const boost::any &a, const engine::string &type,
    const engine::string &file, const engine::string &func, unsigned line, const T &ret = T())
{
//    try
//    {
//        T t = boost::any_cast<T>(a);
//        return t;
//    }
//    catch (const boost::bad_any_cast &e)
//    {
//        engine::log(e.what() + engine::string("\n\tin ") + file + engine::string("\n\tin ") + func +
//            engine::string("\n\tat line ") + boost::lexical_cast<engine::string>(line));
//    }
//    return ret;
    const T *t = boost::any_cast<T>(&a);
    if (!t)
    {
        engine::log("failed any_cast<" + type + ">");
        engine::log("(" + file + " : " + func + " : " + boost::lexical_cast<engine::string>(line) + ")");
        return ret;
    }
    return *t;
}
#define SAFE_ANY_CAST(T, a) safeAnyCast<T>(a, #T, __FILE__, __FUNCTION__, __LINE__)
#define SAFE_ANY_CAST_T(T, a, ret) safeAnyCast<T>(a, #T, __FILE__, __FUNCTION__, __LINE__, ret)

#endif // SAFE_ANY_HPP_INCLUDED
