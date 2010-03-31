 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef PORTAL_JOINT_HPP_INCLUDED
#define PORTAL_JOINT_HPP_INCLUDED

#include <NewtonCustomJoint.h>

#include "types.hpp"

namespace game
{
	/**
	 * @class portalJoing
	 * @brief Represents joint between portals
	 * @see class portal
	 */
    class portalJoint: public NewtonCustomJoint
    {
        public:
			/**
			 * Constructor
			 */
            portalJoint(NewtonBody *child, NewtonBody *parent);
            /**
             * Destructor
             */
            virtual ~portalJoint();

            /// Scale due to relative portal
            float relativeScale;

        protected:
            // Todo: comment submit constraints and submitUserRow
            void SubmitConstraints (dFloat timestep, int threadIndex);
            void SubminUserRow (const dVector& childPin, const dVector& parentPin, dFloat timestep) const;

            dMatrix m_localMatrix0;
            dMatrix m_localMatrix1;
    };
}

#endif // PORTAL_JOINT_HPP_INCLUDED
