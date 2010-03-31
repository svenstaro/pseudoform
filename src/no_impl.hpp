 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef NO_IMPL_HPP_INCLUDED
#define NO_IMPL_HPP_INCLUDED

// TODO: Rename this file, function, including files into longer name!
// Something like not_implemented.hpp and NOT_IMPLEMENTED

/**
 * @def NO_IMPL(x)
 * @brief Used for logging about not implemented stuff
 */
#define NO_IMPL(x) ::engine::log(\
    ::engine::string(__PRETTY_FUNCTION__) +\
    ::engine::string(" not implemented ") +\
    ::engine::string(x))

#endif // NO_IMPL_HPP_INCLUDED
