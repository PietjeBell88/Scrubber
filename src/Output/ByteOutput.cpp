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
#include "ByteOutput.h"

#include "Particles/ParticleArray.h"
#include "Particles/Particle.h"


// Constructor / Destructor
ByteOutput::ByteOutput( const ScrubberParam &param ) :
                          Output( param )
{
    if ( param.output.info != OUTPUT_NOTHING )
    {
        f = fopen( param.output.path.c_str(), "wb" );

        if ( !f )  {
	        printf( "Error in opening output file, exiting\n" );
	        exit( 1 );
        }
    }
}

ByteOutput::~ByteOutput()
{
    if ( param.output.info != OUTPUT_NOTHING )
        fclose( f );
}


// Private Methods
inline void ByteOutput::writePositions( bool first_call, double time, const ParticleArray &particles )
{
    if ( first_call )
    {
        // Write the and delimiter values to file.
        // i.e. buf[] = { xmin, xmax, ymin, ymax, zmin, zmax }
        double buf[] = { radius, height };
        fwrite( buf, 8, 2, f );
    }

    // Frame data
    double buf1[] = { time };
    fwrite( buf1, 8, 1, f );

    int buf2[] = { particles.getLength() };
    fwrite( buf2, 4, 1, f );

    // Write data
    for ( int i = 0; i < particles.getLength(); i++ )
        {
        // Readability
        const Vector2d & pos = particles.getParticle( i ).getPos();

        double buf[] = { pos(0), pos(1) };
        fwrite( buf, 8, 2, f );
    }
}


// Public Methods
void ByteOutput::writeScalarField( const ScalarField &scalar_field )
{
    // Write header
    double buf1[] = { dx, radius };
    fwrite( buf1, 8, 2, f );

    int buf2[] = { scalar_field.shape()(0) };
    fwrite( buf2, 4, 1, f );

    // Write the scalar values to file.
    for ( int i = 0; i < scalar_field.shape()(0); i++ )
    {
        double buf3[] = { scalar_field(i) };
        fwrite( buf3, 8, 1, f );
     }
}