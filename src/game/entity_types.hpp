 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef ENTITY_TYPES_HPP_INCLUDED
#define ENTITY_TYPES_HPP_INCLUDED

namespace game
{
	/// Available game entity types
    enum
    {
        ENTITY_ENTITY,
        ENTITY_GENERIC_PROP,
        ENTITY_PORTAL,
        ENTITY_CHARACTER,
        ENTITY_MARKER,
        ENTITY_TRIGGER,
        ENTITY_PLAYER,
        NUM_ENTITIES
    };
}

#endif // ENTITY_TYPES_HPP_INCLUDED
