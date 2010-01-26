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
#include <fstream>

#include "Typedefs.h"
#include "Scrubber.h"


// Forward Declarations
class Channel;
class ParticleArray;


/**
 * Abstract class for reading and writing to and from files.
 */
class InOut
{
protected:
    OutputInfo outputinfo;

    double height;
    double radius;
    double dx;
    int n;

    FILE *f;

    ScrubberParam param;   // To output the cmdline options to file when wanted.


    /**
     * Write the positions and concentration of particles to the file.
     * @param first_call  True if this function is first called.
     * @param time        Absolute time in seconds.
     * @param particles   Array of particles.
     */
    virtual void writePositions( bool first_call, double time, const ParticleArray &particles ) = 0;

public:
    /**
     * Constructor.
     * @param param  Struct of parameters.
     */
    InOut( const ScrubberParam &param );

    /**
     * Write the concentration of particles to the file.
     * @param first_call  True if this function is first called.
     * @param particles   Array of particles.
     */
    virtual ~InOut();

    /**
     * Write output to file.
     * Based on the choice of the output type, it calls another function that actually writes.
     * @param time       Absolute time in seconds.
     * @param particles  Array of particles.
     */
    void writeToFile( double time, const ParticleArray &particles );

    /**
     * Write the velocity profile to file.
     * @param scalar_field ScalarField containting the velocity profile.
     */
    virtual void writeScalarField( const ScalarField &scalar_field ) = 0;    //FIXME: Should be private (and callable from writeToFile).

    /**
     * Read the velocity profile information from a file.
     * In the process, read the amount of gridpoints and the stepsizes,
     * and write these to the parameter struct.
     * @param *param  Struct of parameters.
     * @param *u      ScalarField to write the velocities to.
     */
    virtual void readProfile( ScrubberParam *param, ScalarField *u ) = 0;
};
