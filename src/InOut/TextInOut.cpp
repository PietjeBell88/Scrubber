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
#include "TextInOut.h"

#include "Particles/ParticleArray.h"
#include "Particles/Particle.h"


// Constructor / Destructor
TextInOut::TextInOut( const ScrubberParam &param ) :
                          InOut( param )
{
    if ( param.output.info != OUTPUT_NOTHING )
    {
        f = fopen( param.output.path.c_str(), "w" );

        if ( !f )  {
	        printf( "Error in opening output file, exiting\n" );
	        exit( 1 );
        }

        // Write file type header
        fwrite( &param.output.format, 4, 1, f );
    }
}

TextInOut::~TextInOut()
{
    if ( param.output.info != OUTPUT_NOTHING )
        fclose(f);
}


// Private Methods
inline void TextInOut::writePositions( bool first_call, double time, const ParticleArray &particles )
{
    if ( first_call )
        fprintf( f, "#T      X      Y      C\n" );

    for ( int i = 0; i < particles.getLength(); i++ )
        {
        // Readability
        const Vector2d & pos = particles.getParticle( i ).getPos();
        const double gram = particles.getParticle( i ).getGramCO2();

        // Write output to file
        fprintf( f, "%e     %e     %e     %e\n",
                 time, pos(0), pos(1), gram );
    }
    fprintf( f, "\n" );
}


// Public Methods
void TextInOut::writeScalarField( const ScalarField &scalar_field )
{
    // Write header
    fprintf( f, "dx = %e, radius = %e, n = %d\n", dx, radius, n );

    // Write the scalar values to file.
    for ( int i = 0; i < scalar_field.shape()(0); i++ )
        fprintf( f, "%e\n", scalar_field(i) );
}

void TextInOut::readProfile( ScrubberParam *param, ScalarField *u )
{
    FILE *f = fopen( param->input.path.c_str(), "r" );
    // Skip the file type header
    fseek ( f , 4 , SEEK_SET );

    // Shut up warnings
    int nread;

    // Read the channel header
    nread = fscanf( f, "dx = %lf, radius = %lf, n = %d\n", &param->channel.dx, &param->channel.radius, &param->channel.n );

    u->resize( param->channel.n + 2 );

    // FIXME: Check if the lenght of the file is sufficient
    // FIXME: Is there a way to not write the elements iteratively?
    for( int i = 0; i < u->shape()(0); i++ )
        nread = fscanf( f, "%lf\n", &(*u)(i) );

    fclose( f );
}
