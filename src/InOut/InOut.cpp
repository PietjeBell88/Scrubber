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
#include "InOut.h"

#include "Typedefs.h"
#include "Particles/ParticleArray.h"
#include "Particles/Particle.h"


// Constructor / Destructor
InOut::InOut( const ScrubberParam &param )
{
    // FIXME: Cast to enum from integer (thanks to parameter parser sucking).
    this->outputinfo = (OutputInfo) param.output.info;

    this->height = param.channel.height;
    this->radius = param.channel.radius;
    this->dx = param.channel.dx;
    this->n = param.channel.n;

    this->param = param; // only needed for outputting cmdline options
}

InOut::~InOut() {}


// Public Methods
void InOut::writeToFile( double time, const ParticleArray &particles )
{
    static bool first_call = true;

    switch ( outputinfo ) {
        case OUTPUT_NOTHING:
            // Do nothing.
            break;
        case OUTPUT_POSITIONS:
            writePositions( first_call, time, particles );
            break;
        default:
            std::cout << "ERROR: Unknown outputtype.";
            break;
    }
    first_call = false;
}
