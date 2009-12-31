 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef XML_CACHE_HPP_INCLUDED
#define XML_CACHE_HPP_INCLUDED

#include <map>

#include "string.hpp"
#include "txml/txml.hpp"

namespace engine
{

    class xmlCache
    {

        private:

            typedef std::map<engine::string, txml::documentPtr> namedDocuments;
            namedDocuments _docs;

        public:

            // fetch a document
            txml::documentPtr loadOrFetch(const engine::string &name)
            {
                // if it doesn't exist, we'll get a null ptr
                // if it does, then we won't get a null ptr
                txml::documentPtr &doc = _docs[name];
                if (!doc)
                {
                    // if it doesn't exist, load it now
                    doc.reset(new txml::document());
                    doc->LoadFile(name.c_str());
                    // if there's an error, just return a null ptr
                    if (doc->Error())
                        doc.reset();
                }
                return doc;
            }

            // clear cache
            void clear()
            {
                // does not invalidate references to the docs
                _docs.clear();
            }

    };



}

#endif // XML_CACHE_HPP_INCLUDED
