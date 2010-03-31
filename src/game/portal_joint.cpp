 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <Newton.h>

#include "portal_joint.hpp"

namespace game
{
    // this is written with the great help of Newton creator, Julio Jerez.  many thanks!

    portalJoint::portalJoint(NewtonBody *child, NewtonBody *parent):
        NewtonCustomJoint(6, child, parent),
        relativeScale(1)
    {
        dMatrix matrix;

        // Use front vector of child matrix as pin
        NewtonBodyGetMatrix(child, &matrix[0][0]);

        dVector pivot(0, 0, 0);
        // Calculate the two local matrix of the pivot point
        ///CalculateLocalMatrix (pivot, matrix.m_front, m_localMatrix0, m_localMatrix1);

        dMatrix pinAndPivot(dgGrammSchmidt(matrix.m_front));
        matrix.m_posit = pivot;
        CalculateLocalMatrix(pinAndPivot, m_localMatrix0, m_localMatrix1);

        NewtonJointSetCollisionState(m_joint, 1);
    }

    portalJoint::~portalJoint()
    {
    }


    void portalJoint::SubminUserRow (const dVector& dir0_, const dVector& dir1_, dFloat timestep) const
    {
        dFloat time;
        dFloat relAccel;
        dVector veloc0;
        dVector veloc1;
        dFloat jacobian0[6];
        dFloat jacobian1[6];

        const dVector dir0 = dir0_;
        jacobian0[0] = dir0.m_x;
        jacobian0[1] = dir0.m_y;
        jacobian0[2] = dir0.m_z;
        jacobian0[3] = 0;
        jacobian0[4] = 0;
        jacobian0[5] = 0;

        dVector dir1 (dir1_.Scale (-1.0f));
        jacobian1[0] = dir1.m_x;
        jacobian1[1] = dir1.m_y;
        jacobian1[2] = dir1.m_z;
        jacobian1[3] = 0;
        jacobian1[4] = 0;
        jacobian1[5] = 0;

        // Add a angular constraint
        NewtonUserJointAddGeneralRow (m_joint, jacobian0, jacobian1);


        time = timestep;
        NewtonBodyGetVelocity(m_body0, &veloc0[0]);
        NewtonBodyGetVelocity(m_body1, &veloc1[0]);
        //!relAccel = -1.0 * (veloc0 % dir0 + veloc1 % dir1) / time;

        // This moves the two bodies at a different Scales in the different spaces.
        // for example if the space one is twice as big the m_ScaleFactor should be 0.5
        // that way when Body in space zero goes at 1 m/s, body in space 1
        // most be moving at 2 m/s do that the relative velocity is zero
        relAccel = -1.0 * (veloc0 % dir0 + (veloc1 % dir1) * relativeScale) / time;

        // Set the desired angular acceleration between the two bodies
        NewtonUserJointSetRowAcceleration (m_joint, relAccel);



        jacobian0[0] = 0;
        jacobian0[1] = 0;
        jacobian0[2] = 0;
        jacobian0[3] = dir0.m_x;
        jacobian0[4] = dir0.m_y;
        jacobian0[5] = dir0.m_z;
        jacobian1[0] = 0;
        jacobian1[1] = 0;
        jacobian1[2] = 0;
        jacobian1[3] = dir1.m_x;
        jacobian1[4] = dir1.m_y;
        jacobian1[5] = dir1.m_z;

        // Add a angular constraint
        NewtonUserJointAddGeneralRow (m_joint, jacobian0, jacobian1);

        NewtonBodyGetOmega(m_body0, &veloc0[0]);
        NewtonBodyGetOmega(m_body1, &veloc1[0]);
        relAccel = -1.0 * (veloc0 % dir0 + veloc1 % dir1) / time;

        // Set the desired angular acceleration between the two bodies
        NewtonUserJointSetRowAcceleration (m_joint, relAccel);
    }

    void portalJoint::SubmitConstraints (dFloat timestep, int threadIndex)
    {
        dMatrix matrix0;
        dMatrix matrix1;

        dMatrix bodyMatrix0;
        dMatrix bodyMatrix1;
        NewtonBodyGetMatrix(m_body0, &bodyMatrix0[0][0]);
        NewtonBodyGetMatrix(m_body1, &bodyMatrix1[0][0]);

        SubminUserRow (bodyMatrix0.m_front, bodyMatrix1.m_front, timestep);
        SubminUserRow (bodyMatrix0.m_up,   bodyMatrix1.m_up, timestep);
        SubminUserRow (bodyMatrix0.m_right, bodyMatrix1.m_right, timestep);
    }
}
