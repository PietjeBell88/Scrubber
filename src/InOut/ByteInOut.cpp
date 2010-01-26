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
#include "ByteInOut.h"

#include "Particles/ParticleArray.h"
#include "Particles/Particle.h"


// Constructor / Destructor
ByteInOut::ByteInOut( const ScrubberParam &param ) :
                          InOut( param )
{
    if ( param.output.info != OUTPUT_NOTHING )
    {
        f = fopen( param.output.path.c_str(), "wb" );

        if ( !f )  {
	        printf( "Error in opening output file, exiting\n" );
	        exit( 1 );
        }

        // Write file type header
        fwrite( &param.output.format, 4, 1, f );
    }
}

ByteInOut::~ByteInOut()
{
    if ( param.output.info != OUTPUT_NOTHING )
        fclose( f );
}


// Private Methods
inline void ByteInOut::writePositions( bool first_call, double time, const ParticleArray &particles )
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
void ByteInOut::writeScalarField( const ScalarField &scalar_field )
{
    // Write header
    double buf1[] = { dx, radius };
    fwrite( buf1, 8, 2, f );

    int buf2[] = { n };
    fwrite( buf2, 4, 1, f );

    // Write the scalar values to file.
    for ( int i = 0; i < scalar_field.shape()(0); i++ )
    {
        double buf3[] = { scalar_field(i) };
        fwrite( buf3, 8, 1, f );
     }
}

void ByteInOut::readProfile( ScrubberParam *param, ScalarField *u )
{
    FILE *f = fopen( param->input.path.c_str(), "rb" );
    // Skip the file type header
    fseek ( f , 4 , SEEK_SET );

    // Read the channel header
    fread( &param->channel.dx,     8, 1, f );
    fread( &param->channel.radius, 8, 1, f );
    fread( &param->channel.n,      4, 1, f );

    u->resize( param->channel.n + 2 );

    // FIXME: Check if the lenght of the file is sufficient
    // FIXME: Is there a way to not write the elements iteratively?
    for( int i = 0; i < u->shape()(0); i++ )
        fread( &(*u)(i), 8, 1, f );

    fclose( f );
}
