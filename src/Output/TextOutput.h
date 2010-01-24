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
#include "Output.h"

#include "Scrubber.h"
#include "Typedefs.h"


/**
 * Writes to the output in humanly readable text form.
 */
class TextOutput : public Output
{
protected:
    virtual void writePositions( bool first_call, double time, const ParticleArray &particles );

public:
    /**
     * Constructor.
     * @param param  Struct of parameters.
     */
    TextOutput( const ScrubberParam &param );

    /**
     * Destructor.
     */
    virtual ~TextOutput();

    //FIXME: Should be private.
    virtual void writeScalarField( const ScalarField &scalar_field );
};