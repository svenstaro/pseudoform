 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <stdexcept>

#include <OgreCamera.h>
#include <OgreRoot.h>
#include <OgreRenderSystem.h>
#include <OgreCommon.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreStringConverter.h>
#include <OgreWindowEventUtilities.h>
#include <OgreManualObject.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>

#include "root.hpp"
#include "data_dir.hpp"
#include "render_lib.hpp"
#include "log.hpp"
#include "fs_ops.hpp"

// parse cfg file
#include "txml/txml.hpp"

namespace engine
{

    namespace gfx
    {

        renderLib_t renderLib = RL_D3D;

        void root::screenShot()
        {
            const string DIR(ROOT_DIR + "screenshots/");
            // make sure that the directory is there
            makeDir(DIR);
            //if (!makeDir(DIR))
            //    log("failed to create screenshot directory");

            _window->writeContentsToTimestampedFile(DIR + "portalized", ".png");
        }

        const string DEFAULT_LOG_FILE_NAME = "ogre.log";

        root::root(const string &cfg, programOptions *opts):
            _root(NULL),
            _window(NULL),
            _renderSys(NULL),
            _sceneMgr(NULL),
            _viewport(NULL),
            _camera(NULL),
            _quad(NULL),
            _windowIsOpen(true)
        {
            txml::document doc(cfg.c_str());
            doc.LoadFile();

            if (doc.Error())
                throw std::runtime_error("root error(" + cfg + "): " + doc.ErrorDesc());

            txml::element *rootElem = doc.RootElement();

            string log = rootElem->attrib<string>("log", DEFAULT_LOG_FILE_NAME);

            _root = new Ogre::Root("", "", log.c_str());
            Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_NORMAL);
            Ogre::LogManager::getSingleton().createLog(LOG_NAME, false, true);

            string lib = rootElem->attrib<string>("lib", "d3d");
            #ifdef _WINDOWS
            string plugins = ".\\"; // windows plugins are in .exe dir
            #else
            // linux plugins are usually in usr/local/lib
            string plugins = rootElem->attrib<string>("plugins");
            #endif

            // load render library plugins
            if (lib == "d3d")
            {
//                #ifdef _DEBUG
//                _root->loadPlugin(plugins + "RenderSystem_Direct3D9_d");
//                #else
                _root->loadPlugin(plugins + "RenderSystem_Direct3D9");
//                #endif
                renderLib = RL_D3D;
            }
            else
            {
//                #ifdef _DEBUG
//                _root->loadPlugin(plugins + "RenderSystem_GL_d");
//                #else
                _root->loadPlugin(plugins + "RenderSystem_GL");
//                #endif
                renderLib = RL_OGL;
            }

            // load CG shader manager and particle manager
//            #ifdef _DEBUG
//            _root->loadPlugin(plugins + "Plugin_CgProgramManager_d");
//            _root->loadPlugin(plugins + "Plugin_ParticleFX_d");
//            #else
            _root->loadPlugin(plugins + "Plugin_CgProgramManager");
            _root->loadPlugin(plugins + "Plugin_ParticleFX");
//            #endif

            Ogre::RenderSystemList *availRenderers = _root->getAvailableRenderers();

            if (availRenderers->empty())
                throw std::runtime_error("root error: no available renderers");

            // choose first one available
            _renderSys = *availRenderers->begin();

            // if we have d3d, set slow mode (see Lua, DirectX and D3DCREATE_FPU_PRESERVE)
            if (lib == "d3d")
            {
                _renderSys->setConfigOption("Floating-point mode", "Consistent");
            }

            _root->setRenderSystem(_renderSys);
            _root->initialise(false);

            size_t width = rootElem->attrib("width", 640);
            size_t height = rootElem->attrib<size_t>("height", 480);

            bool fullscreen = rootElem->attrib<bool>("fullscreen");

            Ogre::NameValuePairList params;
            params["FSAA"] = rootElem->attrib<string>("fsaa", "0");
            params["vsync"] = rootElem->attrib<string>("vsync", "false");
//            params["gamma"] = "true";
            if (opts && std::find(opts->begin(), opts->end(), "-perfHUD") != opts->end())
                params["useNVPerfHUD"] = "true";

            _window = _root->createRenderWindow(
                "pseudoform", width, height, fullscreen, &params);
            _window->setActive(true);
            _window->setAutoUpdated(false);

            Ogre::WindowEventUtilities::addWindowEventListener(_window, this);

            _sceneMgr = _root->createSceneManager(Ogre::ST_GENERIC, "root::_sceneMgr");
            _sceneMgr->setAmbientLight(colour(0.1, 0.1, 0.1));

            bool correct = _sceneMgr->getRootSceneNode()->createChildSceneNode()->getCreator() == _sceneMgr;
            if (correct)
                ::engine::log("yes!");
            else
                ::engine::log("no!");

            _camera = _sceneMgr->createCamera("root::_camera");
            _camera->setNearClipDistance(0.01);
            _camera->setFarClipDistance(1000);
            _camera->setAutoAspectRatio(true);
            _camera->setFOVy(Ogre::Degree(90));

            _viewport = _window->addViewport(_camera);
            _viewport->setBackgroundColour(colour(0.5, 0.5, 0.5, 0));
            _viewport->setClearEveryFrame(false);

            // get rid of default materials
            Ogre::MaterialManager::getSingleton().remove("BaseWhite");
            Ogre::MaterialManager::getSingleton().remove("BaseWhiteNoLighting");

            // load data directory
            Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();
            rgm.addResourceLocation(engine::DATA_DIR, "FileSystem", "data", true);
            rgm.initialiseResourceGroup("data");

//            Ogre::MaterialPtr white = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().load("white",
//                Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME));
//            Ogre::MaterialPtr other = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().load("BaseWhite",
//                Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME));
//            white->copyDetailsTo(other);
//            other = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().load("BaseWhiteNoLighting",
//                Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME));
//            white->copyDetailsTo(other);
            Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(16);
            Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
        }

        root::~root()
        {
            delete _root;
        }

        void root::begin() const
        {
            if (!_windowIsOpen) return;

            Ogre::WindowEventUtilities::messagePump();

            _root->_fireFrameStarted();
            _root->_fireFrameRenderingQueued();

            for (size_t i = 0; i < _window->getNumViewports(); ++i)
            {
                Ogre::Viewport *vp = _window->getViewport(i);
                _renderSys->_setViewport(vp);
                _renderSys->clearFrameBuffer(
                    Ogre::FBT_COLOUR | Ogre::FBT_DEPTH | Ogre::FBT_STENCIL,
                    vp->getBackgroundColour());
            }
        }

        void root::end() const
        {
            if (!_windowIsOpen) return;

            _root->_fireFrameEnded();

            _window->swapBuffers(false);
        }

        size_t root::hwnd() const
        {
            size_t hwnd = 0;
            _window->getCustomAttribute("WINDOW", &hwnd);
            return hwnd;
        }

        unsigned root::width() const
        {
            return _window->getWidth();
        }

        unsigned root::height() const
        {
            return _window->getHeight();
        }

    }


}
