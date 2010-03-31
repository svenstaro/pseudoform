 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef QUAD_HPP_INCLUDED
#define QUAD_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include <OgreColourValue.h>

#include "pre_ogre.hpp"
#include "id.hpp"
#include "types.hpp"

namespace engine
{
    namespace gfx
    {
		/**
		 * @class quad
		 * @brief Shader-based quad rendering
		 *
		 * This class is used for postprocessing
		 */
        class quad
        {
            private:
				/// Quad object
                Ogre::ManualObject *_quad;

                /// Quad rectangle (xywh)
                vec4 _rect;

                /// Relative to screen or pixel based?
                bool _relative;

				/// Quad name
                string _name;

                /// Map of quads names
                static idGen _idGen;

            public:
                /**
                 * Constructor
                 */
                quad(size_t screenW = 1, size_t screenH = 1);

                /**
                 * Destructor
                 */
                ~quad();

                /**
                 * @brief Build quad with given dimensions
                 * @param screenW screen width
                 * @param screenH screen height
                 *
                 * @remarks By default quad is full-screen size
                 */
                void build(size_t screenW, size_t screenH);

                /**
                 * @brief Camera is for corner calculations if wanteds
                 */
                void render(Ogre::Pass *p,
                    Ogre::Viewport *vp,
                    Ogre::Camera *cam = NULL,
                    bool beginEnd = true) const;

                /**
                 * @brief Configure quad object
                 * @param r quad dimensions
                 */
                void rect(const vec4 &r, bool isRelative = false)
                {
                    _rect = r;
                    _relative = isRelative;
                }
        };

        /**
         * Shared pointer to the current class
         */
        typedef boost::shared_ptr<quad> quadPtr;
    }
}

#endif // QUAD_HPP_INCLUDED
