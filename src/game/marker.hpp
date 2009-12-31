 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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

    class marker: public genericProp
    {
        private:

        public:

            static const interfaceType TYPE;

            marker(
                const engine::string &name,
                const genericProp::constructParams &params);
            virtual ~marker();

            virtual void tick(engine::real dt);

            virtual void load(const levelFormat::entity &desc);
            virtual void save(levelFormat::entity &desc);
    };

    class markerFactory: public entityFactory
    {
        private:

            genericProp::constructParams _params;

        public:

            markerFactory(const genericProp::constructParams &params):
                entityFactory(marker::TYPE),
                _params(params)
            {
            }

            entityPtr create(const engine::string &name) const;
    };

    typedef boost::shared_ptr<markerFactory> markerFactoryPtr;

}

#endif // MARKER_HPP_INCLUDED
