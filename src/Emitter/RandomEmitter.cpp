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
#include "RandomEmitter.h"

#include "MTRand.h"

#include "Particles/ParticleArray.h"
#include "Particles/Particle.h"


// Constructor / Destructor
RandomEmitter::RandomEmitter( const ScrubberParam &param ) :
    Emitter( param )
{
    last_emit_time = 0;
}

RandomEmitter::~RandomEmitter() {}


// Public Methods
Vector2d RandomEmitter::startPos( int p )
{
    /*
     * This function directly calculates the starting position from the
     * particle number; this comes in handy when resetting particles, if
     * they leave the cube, to the position they started;
     */
    MTRand myran;

    double x = delimiter(0, 0) + (delimiter(0, 1) - delimiter(0, 0)) * myran.rand53();
    double y = delimiter(1, 0) + (delimiter(1, 1) - delimiter(1, 0)) * myran.rand53();

    return Vector2d( x, y );
}

Vector2d RandomEmitter::startVel( int p )
{
    return init_velocity;
}

void RandomEmitter::init( ParticleArray *particles ) {}

void RandomEmitter::update( double relative_time, ParticleArray *particles )
{
    int to_emit = static_cast<int> ( floor( (relative_time - last_emit_time) * p_rate ) );

    if ( to_emit >= 1 )
        last_emit_time = relative_time;

    while ( (particles->getMaxLength() - particles->getLength()) >= 1
            && to_emit >= 1 )
    {
        particles->add( Particle( startPos( 0 ), startVel( 0 ) ) );
        to_emit--;
    }
}
