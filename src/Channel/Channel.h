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
    double height;
    double radius;

    int n;
    double dx;

    ScalarField u;

    // For filling the velocity profile array.
    CPModel *cpmodel;

    /**
     * Interpolates velocity field at a position.
     * @param v    Velocity field of the channel.
     * @param pos  Position of the particle.
     * @return     Velocity at the position of the particle.
     */
    Vector2d interpolate2d( const Vector2d &pos );

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
     * Initialize the channel.
     * Generates the velocity profile in the channel by looping until steady.
     */
    void init();

    /**
     * Checks if the particle has left the box on the sides, top, bottom or is still in the box.
     * @param pos Position of the particle.
     * @return    An enum indicating where the particle is at.
     */
    PosBox outsideBox( const Vector2d &pos );

    /**
     * Get the velocity in the channel at a certain position.
     * @param pos  Position to get the velocity at.
     * @return     The velocity of the continuous phase at the position pos.
     */
    Vector2d velocityAt( const Vector2d &pos );

    /**
     * Get the velocity field.
     * @return  The vector field.
     */
    const ScalarField &getVelocityField() const;
};
