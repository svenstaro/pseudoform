 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef MARKER_HPP_INCLUDED
#define MARKER_HPP_INCLUDED

#include "generic_prop.hpp"

namespace game
{
    class marker;
    typedef boost::shared_ptr<marker> markerPtr;

    /**
     * @class marker
     * @brief Used for making game markers
     * @see class genericProp, class entity
     */
    class marker: public genericProp
    {
        private:
			// Empty

        public:
            static const interfaceType TYPE;

            /**
             * Constructor
             */
            marker(
                const engine::string &name,
                const genericProp::constructParams &params);
            virtual ~marker();

            virtual void tick(engine::real dt);

            virtual void load(const levelFormat::entity &desc);
            virtual void save(levelFormat::entity &desc);
    };

    /**
     * @class markerFactory
     * @see class entityFactory
     * @remarks for more information
     * visit: http://en.wikipedia.org/wiki/Factory_method_pattern
     */
    class markerFactory: public entityFactory
    {
        private:
            genericProp::constructParams _params;

        public:
			/**
			 * Constructor
			 */
            markerFactory(const genericProp::constructParams &params):
                entityFactory(marker::TYPE),
                _params(params)
            {
            }

            /**
             * @brief create marker for the factory
             * @param name new marker name
             */
            entityPtr create(const engine::string &name) const;
    };

    /**
     * Shared pointer to the current class
     */
    typedef boost::shared_ptr<markerFactory> markerFactoryPtr;
}

#endif // MARKER_HPP_INCLUDED
