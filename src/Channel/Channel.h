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


// Forward Declarations
class CPModel;


/**
 * Class that handles the geometry of the channel, and stores the continuous phase in it.
 */
class Channel
{
protected:
    double dt;

    double height;
    double radius;

    double fldensity;

    double co2_density;

    double conc_b;
    double conc_t;

    int n;
    double dx;

    TurbModel turb_model;

    ScalarField u;

    // For filling the velocity profile array.
    CPModel *cpmodel;

    /**
     * Interpolates velocity component u at a certain position.
     * @param pos  Position of the particle.
     * @return     Velocity u at the position of the particle.
     */
    double interpolate2d( const Vector2d &pos );

    /**
     * Gets the derivative of the velocity componenent u in the y direction at a certain position.
     * @param pos  Position of the particle.
     * @return     Derivative of the velocity compenent u in the y direction.
     */
    double dudy( const Vector2d &pos );

public:
    /**
     * Conctructor.
     * @param param  Struct of parameters.
     */
	Channel( const ScrubberParam &param );

    /**
     * Destructor.
     */
    virtual ~Channel();

    /**
     * Initialize the channel by calculating the velocity profile.
     */
    void init();

    /**
     * Initialize the channel by copying in an existing velocity profile.
     * @param u  The ScalarField to be used as the velocity profile.
     */
    void init( const ScalarField &u );

    /**
     * Checks if the particle has left the box on the sides, top, bottom or is still in the box.
     * @param pos Position of the particle.
     * @return    An enum indicating where the particle is at.
     */
    PosBox outsideBox( const Vector2d &pos );

    /**
     * Get a surrouding fluid velocity of a particle in the channel based on the Discrete Eddy Model.
     * @param pos          Position of the particle.
     * @param vel          Velocity of the particle.
     * @param *v_vel       Calculated velocity of the surrounding fluid.
     * @param *count_down  Calculated count_down.
     */
    void velocityAt( const Vector2d &pos, const Vector2d &vel, Vector2d *v_vel, double *count_down );

    /**
     * Get the velocity field.
     * @return  The vector field.
     */
    const ScalarField &getVelocityField() const;

   /**
    * Gives the mass fraction of CO2 at a certain position.
    * @param pos  Position to get the mass fraction at.
    * @return     The mass fraction of CO2 at pos.
    */
    double massFracAt( const Vector2d &pos );
};
