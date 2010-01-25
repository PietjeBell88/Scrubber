// Copyright (c) 2009, Pietje Bell <pietjebell@ana-chan.com>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#pragma once

// Headers
#include "Typedefs.h"
#include "Scrubber.h"


/**
 * A class providing various methods to calculate a discrete velocity field.
 */
class CPModel
{
protected:
    // Fluid properties (readability)
    double mu;
    double rho;
    double nu;

    // Grid properties (readability)
    double dy;
    double diameter;
    double delta;
    int n;                   /// Number of volumes IN the pipe. Please remember that there are also (half) volumes outside the pipe.

    // Model
    LoopModel loop_model;
    double errork;
    double relax;

    // Boundary conditions
    GlobalBC globbc;
    double globbv;

    WallBC wallbc;
    double wallbv;

    double pg;       /// Pressure gradient (usually negative)

    // Other stuff
    ScalarField y_mu;        /// Array of y values at mu nodes.

    // Prandtl mixing length constants
    static const double lambda;
    static const double kappa;

    /**
     * Get the velocity profile by calling one of the loop methods.
     * @param u  Profile which will be filled.
     * @return   Velocity profile.
     */
    ScalarField CPModel::velocityProfile( ScalarField u );

    /**
     * Use the simple model to get a steady solution.
     * @param u  Profile which will be calculated.
     * @return   Calculated profile.
     */
    ScalarField loopSimple( ScalarField u );

    /**
     * Use the prandtl mixing length model to get a steady solution.
     * @param u  Profile which will be calculated.
     * @return   Calculated profile.
     */
    ScalarField loopPrandtl( ScalarField u );

    /**
     * Calculates the error between two ScalarFields based on the frobenius norm.
     * @param new_field  New ScalarField.
     * @param old_field  Old ScalarField.
     * @return           The relative error between the two ScalarFields.
     */
    double froNorm( const ScalarField &new_field, const ScalarField &old_field );

    /**
     * Sets the ghost points.
     * Sets the ghost points to satisfy the wall boundary condition.
     * @param u  The ScalarField of velocities.
     */
    void setGhost( ScalarField *u );

    /**
     * Derivative of the velocity.
     * Calculates the derivative of the velocity between two velocity points
     * (which is where the viscosity points are usually located).
     * @param v  The ScalarField of velocities.
     * @param i  Index of the viscosity point at which dudy should be calculated.
     * @return   Derivative of the velocity.
     */
    inline double dudy( const ScalarField &u, int i )
    {
        return (u(i+1) - u(i))/dy;
    }

public:
    /**
     * Conctructor.
     * @param param  Struct of parameters.
     */
    CPModel( const ScrubberParam &param );

    /**
     * Destructor.
     */
    virtual ~CPModel();

    /**
     * Initialize a ScalarField with values.
     * Generates the velocity profile in the channel by looping until steady, and
     * makes sure the global boundary condition is satisfied.
     */
    ScalarField init( ScalarField u );

    /**
     * Calculate the Prandtl mixing length.
     * @param y     Height at which the mixing length should be calculated.
     * @return      The Prandtl mixing length.
     */
    double CPModel::prandtlLength( double y );
};
