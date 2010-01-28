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


// Headers
#include "GridOnceEmitter.h"

#include "Particles/ParticleArray.h"
#include "Particles/Particle.h"

#include "Channel/Channel.h"


// Constructor / Destructor
GridOnceEmitter::GridOnceEmitter( const ScrubberParam &param, Channel *channel ) :
    Emitter( param, channel )
{}

GridOnceEmitter::~GridOnceEmitter() {}


// Public Methods
Vector2d GridOnceEmitter::startPos( int p )
{
    // What variables are used in this function?

    /*
     * This function directly calculates the starting position from the
     * particle number; this comes in handy when resetting particles, if
     * they leave the cube, to the position they started;
     */
    int i = p / grid(1);
    int j = p % grid(1);

    return Vector2d( delimiter(0, 0) + i * dx,
                     delimiter(1, 0) + j * dy );
}

Vector2d GridOnceEmitter::startVel( int p )
{
    return init_velocity;
}

void GridOnceEmitter::init( ParticleArray *particles )
{
    for ( int p = 0; p < p_N; p++ )
    {
        const Vector2d pos = startPos( p );
        const Vector2d vel = startVel( p );

        if ( channel->outsideBox( pos ) == P_INSIDE )
            particles->add( Particle( pos, vel ) );
    }
}

void GridOnceEmitter::update( double relative_time, ParticleArray *particles )
{}
