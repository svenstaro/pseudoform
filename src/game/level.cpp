 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <fstream>
#include <vector>

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>

#include <boost/foreach.hpp>
//#include "txml/txml.hpp"

#include "script/bindings.hpp"
#include "script/script.hpp"

//#include "gui/sheet.hpp"

#include "level.hpp"
#include "types.hpp"
#include "world.hpp"
#include "entity.hpp"
#include "generic_prop.hpp"

#include "portal.hpp"
#include "trigger.hpp"
#include "marker.hpp"
#include "character.hpp"
#include "player.hpp"


#include "log.hpp"
#include "data_dir.hpp"
#include "gfx/renderer.hpp"

#include "protobuf/level_format.pb.h"

namespace game
{

    level::level(engine::gfx::renderer &renderer,
        engine::snd::system &sndSys, bool inGame):
        _renderer(renderer),
        _soundSys(sndSys)
    {
        // Set up scripts
        engine::script::bindCommon(script);
        engine::script::bindGame(script);
        engine::script::bindMath(script);
        engine::script::bindPhys(script);
        engine::script::bindActionBinder(script);
        engine::script::bindGUI(script);
        engine::script::bindSnd(script);
        engine::script::bindEditor(script);
        engine::script::bindPlay(script);

        _regFactories(inGame);
    }

    level::~level()
    {
        clear();
    }

    void level::_regFactories(bool inGame)
    {
        genericProp::constructParams gpParams(
            _renderer.sceneMgr()->getRootSceneNode(),
            physWorld,
            _soundSys,
            inGame);

        // Register all stuff factories
        entFactory.add(genericPropFactoryPtr(new genericPropFactory(gpParams)));
        entFactory.add(characterFactoryPtr(new characterFactory(gpParams)));
        entFactory.add(portalFactoryPtr(new portalFactory(gpParams)));
        entFactory.add(triggerFactoryPtr(new triggerFactory(gpParams, script)));
        entFactory.add(markerFactoryPtr(new markerFactory(gpParams)));
        entFactory.add(playerFactoryPtr(new playerFactory(gpParams)));
    }

    void level::reset()
    {
        gameWorld.reset();
        physWorld.resetAccum();

        // TODO uncomment level stop function when script will be done
//        engine::script::callFunc(script, "levelStop", boost::ref(*this));
    }

    void level::clear()
    {
        gameWorld.clear();
        scriptName = "";
    }

    void level::start()
    {
        if (scriptName.empty())
            return;

        // TODO: uncomment when done level starting script
//        if (!script.file(engine::DATA_DIR + script))
//        {
//            engine::log(script + " error: ");
//            engine::log(script.error());
//        }

//        engine::script::callFunc(script, "levelStart", boost::ref(*this));
    }

    void level::tick(float dt)
    {
        physWorld.tick(dt);
        gameWorld.tick(dt);
    }

    void level::load(const engine::string &fileName)
    {
        std::ifstream in((engine::DATA_DIR + fileName).c_str(), std::ios_base::binary);
        if (!in.good())
        {
            engine::log("Failed to load level " + fileName);
            return;
        }

        levelFormat::level levelData;
        levelData.ParseFromIstream(&in);

        if (levelData.has_script())
            scriptName = levelData.script();
        else
            scriptName.clear();

        // First pass: create the entities
        std::vector<entityPtr> ents;
        for (int i = 0; i < levelData.entities_size(); ++i)
        {
            const levelFormat::entity &entData = levelData.entities(i);

            if (!entData.has_name() || !entData.has_type())
            {
                engine::log("Entity in level " + fileName + " has no type/name");
                continue;
            }

            //engine::log("creating a " + entData.type() + " with name " + entData.name());

            entityPtr ent = entFactory.create(entData.type(), entData.name());
            ents.push_back(ent);

            gameWorld.add(ent);
        }

        // Second pass: load the entities
        // we do this because if any of the entities are linked
        // then they will require the other entities to be created first
        for (int i = 0; i < levelData.entities_size(); ++i)
        {
            const levelFormat::entity &entData = levelData.entities(i);
            ents[i]->load(entData);
            ents[i]->setCurrentStateAsInitial();
        }
    }

    void level::save(const engine::string &fileName)
    {
        levelFormat::level levelData;

        levelData.set_script(scriptName);

        BOOST_FOREACH(const entityList::value_type &i, gameWorld.list())
        {
            const entityPtr &ent = i.second;

            levelFormat::entity *entData = levelData.add_entities();
            ent->save(*entData);
        }

        std::ofstream out((engine::DATA_DIR + fileName).c_str(), std::ios_base::binary);
        if (!out.good())
        {
            engine::log("Failed to save level " + fileName);
        }
        else
        {
            levelData.SerializeToOstream(&out);
        }
    }
}
