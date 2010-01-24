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
#include "TextOutput.h"

#include "Particles/ParticleArray.h"
#include "Particles/Particle.h"


// Constructor / Destructor
TextOutput::TextOutput( const ScrubberParam &param ) :
                          Output( param )
{
    if ( param.output.info != OUTPUT_NOTHING )
    {
        f = fopen( param.output.path.c_str(), "w" );

        if ( !f )  {
	        printf( "Error in opening output file, exiting\n" );
	        exit( 1 );
        }
    }
}

TextOutput::~TextOutput()
{
    if ( param.output.info != OUTPUT_NOTHING )
        fclose(f);
}


// Private Methods
inline void TextOutput::writePositions( bool first_call, double time, const ParticleArray &particles )
{
    if ( first_call )
        fprintf( f, "#T      X      Y      C\n" );

    for ( int i = 0; i < particles.getLength(); i++ )
        {
        // Readability
        const Vector2d & pos = particles.getParticle( i ).getPos();

        // Write output to file
        fprintf( f, "%e     %e     %e\n",
                 time, pos(0), pos(1) );
    }
    fprintf( f, "\n" );
}


// Public Methods
void TextOutput::writeScalarField( const ScalarField &scalar_field )
{
    // Write header
    fprintf( f, "dx = %e, radius = %e, n+2 = %d\n", dx, radius, scalar_field.shape()(0) );

    // Write the scalar values to file.
    for ( int i = 0; i < scalar_field.shape()(0); i++ )
        fprintf( f, "%e\n", scalar_field(i) );
}