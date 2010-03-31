 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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
    /**
    * @class xmlCache
    */
    class xmlCache
    {
        private:
            typedef std::map<engine::string, txml::documentPtr> namedDocuments;
            /// List of loaded xml-documents
            namedDocuments _docs;

        public:
            /**
            * @brief Fetch a document
            * @param name name of the document which try to load
            * @return null if document doesn't exist
            */
            txml::documentPtr loadOrFetch(const engine::string &name)
            {
                txml::documentPtr &doc = _docs[name];
                if (!doc)
                {
                    // If it doesn't exist, load it now
                    doc.reset(new txml::document());
                    doc->LoadFile(name.c_str());

                    // If there's an error, just return a null ptr
                    if (doc->Error())
                        doc.reset();
                }
                return doc;
            }

            /**
            * @brief Clear cache
            */
            void clear()
            {
                // Does not invalidate references to the docs
                _docs.clear();
            }
    };
}

#endif // XML_CACHE_HPP_INCLUDED
