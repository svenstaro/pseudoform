 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef BINDINGS_HPP_INCLUDED
#define BINDINGS_HPP_INCLUDED

namespace engine
{

    namespace script
    {

        class script;

        //void prepare(script &scr);
        void bindMath(script &scr);
        void bindPhys(script &scr);
        void bindGfx(script &scr);
        void bindGame(script &scr);
        void bindCommon(script &scr);
        void bindActionBinder(script &scr);
        void bindGUI(script &scr);
        void bindSnd(script &scr);
        void bindEditor(script &scr);
        void bindPlay(script &scr);

    }

}

#endif // BINDINGS_HPP_INCLUDED
