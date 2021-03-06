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

//Headers
#include "Emitter.h"

#include "Typedefs.h"
#include "Scrubber.h"


/**
 * Emits particles every <time interval>.
 */
class GridEmitter: public Emitter
{
protected:
    double last_emit_time; // Relative time at which the last particles were emitted

    // Keep track of the particles that were supposed to be emitted outside the channel.
    int left_over;

public:
    GridEmitter( const ScrubberParam &param, Channel *channel );

    virtual ~GridEmitter();

    virtual void init( ParticleArray *particles );

    virtual void update( double relative_time, ParticleArray *particles );

    virtual Vector2d startPos( int p );

    virtual Vector2d startVel( int p );
};
