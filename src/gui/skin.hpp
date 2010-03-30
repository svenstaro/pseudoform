 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SKIN_HPP_INCLUDED
#define SKIN_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include "types.hpp"
#include "skin_pre.hpp"

namespace engine
{
    namespace gui
    {
		/**
		 * Gui skin class
		 */
        class skin
        {
            private:
				/// Gui skin name
                string _name, _prefix;

                /// For caching getTexture() calls
                mutable std::map<string, bool> _textures;

            public:
				/**
				 * Constructor
				 */
                skin(const string &name);

                /**
                 * Destructor
                 */
                ~skin();

                /**
                 * The part of skin
                 */
                struct piece
                {
                    string tex;
                    vec2 size;
                };

                // TODO
                typedef std::map<string, piece> pieceList;
                typedef std::map<string, pieceList> elementMap;

                const piece &getPiece(const string &type, const string &align) const;

                string getTexture(const string &tex) const;

            private:
                static piece _defaultPiece;
                elementMap _elements;
        };
    }
}

#endif // SKIN_HPP_INCLUDED
