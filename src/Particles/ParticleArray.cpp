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
#include "ParticleArray.h"


// Constructor / Destructor
ParticleArray::ParticleArray( int initiallength )
{
    particles.resize( initiallength );

    length = 0;
    nextIndex = 0;
}


// Public methods
void ParticleArray::add( const Particle &particle )
{
    particles(length) = particle;
    length++;
    nextIndex++;
}

Particle ParticleArray::remove( int p )
{
    Particle temp = particles( p );
    particles( p ) = particles( length - 1 ); // [ 1 2 3 4 5 ] at length 5, with particle nr 2 (index 1) outside of the box becomes
    length--;                                 // [ 1 5 3 4 5 ] with length 4;
    return temp;
}


// Getters and Setters
const Particle &ParticleArray::getParticle( int p ) const
{
    return particles( p );
}

void ParticleArray::setParticle( int p, Particle particle )
{
    this->particles( p ) = particle;
}

int ParticleArray::getLength() const
{
    return length;
}

int ParticleArray::getMaxLength() const
{
    return particles.size();
}
