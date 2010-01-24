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
#include "Emitter.h"

#include "Particles/ParticleArray.h"
#include "Particles/Particle.h"


// Constructor / Destructor
Emitter::Emitter( const ScrubberParam &param )
{
    // Save the variables (readability)
    this->p_density = param.p.density;
    this->p_diameter = param.p.radius * 2;
    this->init_velocity = param.emitter.init_velocity;

    this->grid = param.emitter.grid;
    this->delimiter = param.emitter.delimiter;
    this->dx = param.emitter.dx;
    this->dy = param.emitter.dy;
    this->p_N = param.emitter.p_N;

    this->p_rate = param.emitter.rate;
}

Emitter::~Emitter() {}
