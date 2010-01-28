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
#include <string.h>

#include "Typedefs.h"
#include "Scrubber.h"


// Forward Declarations
class ParticleArray;


// Namespace
using std::string;


/**
 * Abstract class for emitting particles.
 */
class Emitter
{
protected:
    // Particle properties (readability)
    double p_density;
    double p_diameter;

    TGrid grid;
    TDelimiter delimiter;
    double dx, dy, dz;

    int p_N;
    double p_rate;
    Vector2d init_velocity;

    Channel *channel;

    /**
     * Returns the start position of a particle.
     * @param p  Number of the particle (i.e. emit position can be particle number dependant)
     */
    virtual Vector2d startPos( int p ) = 0;

    /**
     * Returns the start velocity of a particle.
     * @param p  Number of the particle (i.e. emit velocity can be particle number dependant)
     */
    virtual Vector2d startVel( int p ) = 0;

public:
    /**
     * Constructor.
     * @param param     Struct of parameters.
     * @param *channel  The channel with its dimensions.
     */
    Emitter( const ScrubberParam &param, Channel *channel );

    /**
     * Destructor.
     */
    virtual ~Emitter();

    /**
     * Initializes the emitter, and possible emit particles.
     * @param particles  Array that will hold the emitted particles.
     */
    virtual void init( ParticleArray *particles ) = 0;

    /**
     * Updates the emitter, and possible emit new particles.
     * @param time       Absolute time in seconds.
     * @param particles  Array that will hold the emitted particles.
     */
    virtual void update( double time, ParticleArray *particles ) = 0;
};
