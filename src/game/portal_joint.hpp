 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef PORTAL_JOINT_HPP_INCLUDED
#define PORTAL_JOINT_HPP_INCLUDED

#include <NewtonCustomJoint.h>

#include "types.hpp"

namespace game
{

    class portalJoint: public NewtonCustomJoint
    {
        public:

            portalJoint(NewtonBody *child, NewtonBody *parent);
            virtual ~portalJoint();

            float relativeScale;

        protected:

            void SubmitConstraints (dFloat timestep, int threadIndex);
            void SubminUserRow (const dVector& childPin, const dVector& parentPin, dFloat timestep) const;

            dMatrix m_localMatrix0;
            dMatrix m_localMatrix1;
    };

}

#endif // PORTAL_JOINT_HPP_INCLUDED
