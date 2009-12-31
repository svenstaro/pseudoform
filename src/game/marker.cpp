 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include "phys/body.hpp"
#include "phys/world.hpp"

#include "entity.hpp"
#include "marker.hpp"
#include "data_dir.hpp"
#include "safe_any.hpp"

#include "entity_types.hpp"
#include "protobuf/level_format.pb.h"

namespace game
{
    const interfaceType marker::TYPE = ENTITY_MARKER;

    marker::marker(
        const engine::string &name,
        const genericProp::constructParams &params):
        genericProp(name, params)
    {
        _interfaces.push_back(marker::TYPE);

        visible(!params.inGame);
        active(!params.inGame);
    }

    marker::~marker()
    {
    }

    void marker::tick(engine::real dt)
    {
        genericProp::tick(dt);
    }

    void marker::load(const levelFormat::entity &desc)
    {
        genericProp::load(desc);
    }

    void marker::save(levelFormat::entity &desc)
    {
        genericProp::save(desc);
    }

    entityPtr markerFactory::create(const engine::string &name) const
    {
        using namespace engine;

        markerPtr ent(new marker(name, _params));

        return ent;
    }

}

