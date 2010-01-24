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
#include "GridEmitter.h"

#include "Particles/ParticleArray.h"
#include "Particles/Particle.h"


// Constructor / Destructor
GridEmitter::GridEmitter( const ScrubberParam &param ) :
    Emitter( param )
{
    last_emit_time = 0;
}

GridEmitter::~GridEmitter() {}


// Public Methods
Vector2d GridEmitter::startPos( int p )
{
    int i = p / grid(1);
    int j = p % grid(1);

    return Vector2d( delimiter(0, 0) + i * dx,
                     delimiter(1, 0) + j * dy );
}

Vector2d GridEmitter::startVel( int p )
{
    return init_velocity;
}

void GridEmitter::init( ParticleArray *particles )
{
    for ( int p = 0; p < p_N; p++ )
        particles->add( Particle( startPos( p ), startVel( p ) ) );
}

void GridEmitter::update( double relative_time, ParticleArray *particles )
{
    // Saving up particles until there are enough saved up to emit in the grid:
    if ( (relative_time - last_emit_time) * p_rate >= p_N
            && (particles->getMaxLength() - particles->getLength()) >= p_N )
    {
        last_emit_time = relative_time;

        // Emit:
        for ( int p = 0; p < p_N; p++ )
            particles->add( Particle( startPos( p ), startVel( p ) ) );
    }
}
