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


/**
 * Contains all properties a particle should have.
 */
class Particle
{
private:
    Vector2d pos;
    Vector2d vel;

public:
    /**
     * Default constructor (only needed for creation of ParticleArray).
     */
    Particle();

    /**
     * Constructor.
     * @param pos  Initial position of the particle.
     * @param vel  Initial velocity of the particle.
     */
    Particle( const Vector2d &pos, const Vector2d &vel );

    /**
     * Get the particle position.
     * @return  The position of the particle.
     */
    const Vector2d &getPos() const;

    /**
     * Set the particle position.
     * @param pos  New position.
     */
    void setPos( const Vector2d &pos );

    /**
     * Get the particle velocity.
     * @return  The velocity of the particle.
     */
    const Vector2d &getVel() const;

    /**
     * Set the particle velocity.
     * @param vel  New velocity.
     */
    void setVel( const Vector2d &vel );
};
