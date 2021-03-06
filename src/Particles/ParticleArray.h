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
#include <blitz/array.h>

#include "Typedefs.h"
#include "Particle.h"


/**
 * Holds and manages the particles.
 */
class ParticleArray
{
private:
    blitz::Array<Particle, 1> particles; /// The array for the particles.

    int length;    /// Keeps track of how many particles there are.
    int nextIndex; /// Contains the index of the next particle when added.

public:
    /**
     * Constructor.
     * @param initiallength  Initial length of the particle array.
     */
    ParticleArray( int initiallength );

    /**
     * Add a particle to the array.
     * @param particle  The particle which will be added to the array.
     */
    void add( const Particle &particle );

    /**
     * Remove a particle from the array.
     * @param p  Index of the particle in the array.
     * @return   Removed particle.
     */
    Particle remove( int p );

    /**
     * Get particle p.
     * @param p  Index of the particle in the array.
     * @return   The particle at position p in the array.
     */
    const Particle &getParticle( int p ) const;

    /**
     * Write particle to array.
     * @param p         Index of the particle in the array.
     * @param particle  Particle which will be written to index p.
     */
    void setParticle( int p, Particle particle );

    /**
     * Get the current array length.
     * @return  The current array length.
     */
    int getLength() const;

    /**
     * Get the maximal array length.
     * @return  The maximal array length.
     */
    int getMaxLength() const;
};
