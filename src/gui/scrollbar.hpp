 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SCROLLBAR_HPP_INCLUDED
#define SCROLLBAR_HPP_INCLUDED

#include "panel.hpp"

namespace engine
{
    namespace gui
    {

        class dragger: public panel
        {
            private:

                vec2 _dir;
                float _len, _percent;

                vec2 endPos() const;

            public:

                dragger(const string &name, bool internal = false);
                ~dragger();

                void drag(const vec2 &v);

                float percent() const { return _percent; }
                void percent(float f);

                void fixDirAndLen();
        };

        typedef boost::shared_ptr<dragger> draggerPtr;

        class scrollbar: public panel
        {

            private:

                draggerPtr _dragger;

                void _scroll(const event &e);

            public:

                draggerPtr getDragger() const { return _dragger; }

                void points(const vec2 &a, const vec2 &b);

                float percent() const { return _dragger->percent(); }
                void percent(float f) { _dragger->percent(f); }

                scrollbar(const string &name, bool internal = false);
                ~scrollbar();

        };

        typedef boost::shared_ptr<scrollbar> scrollbarPtr;


    }
}

#endif // SCROLLBAR_HPP_INCLUDED
