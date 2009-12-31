 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef PARAMS_HPP_INCLUDED
#define PARAMS_HPP_INCLUDED


#include <map>

#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

#include "string.hpp"
//#include "safe_any.hpp"
#include "log.hpp"

//! deprecated
//namespace engine
//{
//
//    //using boost::any_cast;
//    using boost::any;
//    using boost::optional;
//
//    class param
//    {
//        private:
//
//            string _name;
//            any _value;
//
//        public:
//
//            param(): _name("(invalid)")
//            {
//            }
//
//            param(const string &name, const any &value):
//                _name(name), _value(value)
//            {
//            }
//
//            param(const param &rhs):
//                _name(rhs._name), _value(rhs._value)
//            {
//            }
//
//            const string &name() const { return _name; }
//
//            template<typename T>
//            T as(const string &type, const string &file, const string &func, unsigned line) const
//            {
//                const T *ptr = boost::any_cast<T>(&_value);
//
//                if (!ptr)
//                {
//                    log("failed to fetch param \"" + _name + "\" as type \"" + type + "\"");
//                    log("(" + file + " : " + func + " : " + boost::lexical_cast<string>(line) + ")");
//                    return T();
//                }
//
//                return *ptr;
//            }
//
//            // trailing underscore to avoid conflicts
//            #define AS_(T) as<T>(#T, __FILE__, __PRETTY_FUNCTION__, __LINE__)
//    };
//
//    typedef optional<param> optParam;
//
//    class parameters
//    {
//        private:
//
//            typedef std::map<string, param> map;
//
//            map _params;
//
//        public:
//
//            void clear()
//            {
//                _params.clear();
//            }
//
//            void set(const string &name, const any &val)
//            {
//                _params[name] = param(name, val);
//            }
//
//            optParam get(const string &name) const
//            {
//                map::const_iterator i = _params.find(name);
//
//                if (i != _params.end())
//                    return i->second;
//
//                return optParam();
//            }
//
//            typedef map::value_type value_type;
//
//            const map &list() const { return _params; }
//
//            #define PARAM_ITER_ const ::engine::parameters::value_type &_paramIter
//            #define FIRST_CASE_(name) if (_paramIter.first == name)
//            #define CASE_(name) else if (_paramIter.first == name)
//            #define PARAM_ _paramIter.second
//    };
//
////    typedef std::map<string, any> parameters;
//    typedef boost::shared_ptr<parameters> parametersPtr;
//
////    class parameter
////    {
////        private:
////
////            parameter() {}
////
////        public:
////
////            typedef parameters::const_iterator iter;
////
////            parameter(const iter &end):
////                _end(end) {}
////
////            parameter(const iter &i, const iter &end):
////                _iter(i), _end(end) {}
////
////            parameter(const parameter &o):
////                _iter(o._iter), _end(o._end) {}
////
////            parameter &operator=(const iter &i)
////            {
////                _iter = i;
////                return *this;
////            }
////
////            bool valid() const
////            {
////                return _iter != _end;
////            }
////
////            operator bool() const
////            {
////                return valid();
////            }
////
////            const any &operator()() const
////            {
////                return _iter->second;
////            }
////
//////            template<typename T>
//////            T value() const
//////            {
//////                //return any_cast<T>(_iter->second);
//////                return SAFE_ANY_CAST(T, _iter->second);
//////            }
////
//////            const iter &get() const
//////            {
//////                return _iter;
//////            }
////
////        private:
////            iter _iter, _end;
////    };
//
////    inline bool valid(const parameter &param, const parameters &params)
////    {
////        return param != params.end();
////    }
////
////    inline any &value(const parameter &param)
////    {
////        return param->second;
////    }
//
//}

#endif // PARAMS_HPP_INCLUDED
