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
#include "Scrubber.h"

#include <stdio.h>

#include "getopt_pp.h"

#include "Output/Output.h"
#include "Output/ByteOutput.h"
#include "Output/TextOutput.h"

#include "Emitter/Emitter.h"
#include "Emitter/GridEmitter.h"
#include "Emitter/GridOnceEmitter.h"
#include "Emitter/RandomEmitter.h"

#include "Channel/Channel.h"

#include "Particles/Mover.h"
#include "Particles/ParticleArray.h"
#include "Particles/Particle.h"


// Using
using std::string;
using std::cout;


inline void writeProgress( int perc )
{
    printf( "[%d%%]\r", perc );
    fflush( stdout);
}

int main( int argc, char* argv[] )
{
    // Parameters
    ScrubberParam param;

    // Parse the parameters
    parse( argc, argv, &param );
    printParam( param );

    // Making the Output
    Output * output;

    switch ( param.output.format ) {
        case OUTPUT_BYTE:
            output = new ByteOutput( param );
            break;
        case OUTPUT_TEXT:
            output = new TextOutput( param );
            break;
        default:
            cout << "Unknown output type.";
            break;
    }

    // Making the channel
    Channel *channel = new Channel( param );

    channel->init();

    if ( param.output.info == OUTPUT_VELFIELD )
    {
        output->writeScalarField( channel->getVelocityField() );
        delete output;
        delete channel;
        return 0;
    }

    // Making the Emitter
    Emitter *emitter;

    switch ( param.emitter.type ) {
        case EMITTER_ONCE:
            emitter = new GridOnceEmitter( param );
            break;
        case EMITTER_GRID:
            emitter = new GridEmitter( param );
            break;
        case EMITTER_RANDOM:
            emitter = new RandomEmitter( param );
            break;
        default:
            cout << "Unknown Emitter type";
            exit( 1 );
    }

    // Making the particle mover
    Mover * mover;

    mover = new Mover( param, channel );

    // Allocating memory for the array that holds the particles
    ParticleArray particles( param.maxparticles );

    // Emit the particles
    emitter->init( &particles );

    double time = 0;
    output->writeToFile( time, particles );
    double time_next_output = param.output.interval;

    while ( time <= param.duration )
    {
        writeProgress( 100 * time / param.duration );

        // Move the particles
        mover->doMove( &particles );

        // Write to file
        if ( time >= time_next_output )
        {
            output->writeToFile( time, particles );
            time_next_output += param.output.interval;
        }

        emitter->update( time, &particles );

        time += param.dt;

        // Break when there are no more particles to plot.
        if ( particles.getLength() == 0 )
            break;
    }

    delete output;
    delete mover;
    delete emitter;
    delete channel;
    return 0;
}

// Help
void show_help()
{
    printf(
            "General Options:\n"
            "  -h, --help                                  Produce this help message.\n"
            "      --duration <double> (=600.0)            Duration of computation in seconds.\n"
            "      --dtscale <double> (=0.5)               dt = dtscale * systemtime.\n"
            "      --errork <double> (=1E-5)               The error threshold for the steady velocity profile.\n"
            "      --relax <double> (=0.9)                 Relaxation for prandtl mixing length. 0 = none, 0.99 = a lot.\n"
            "      --gravangle <double> (=0)               Angle of gravity with the negative z-axis.\n"
            "      --maxp <int> (=1000)                    Maximum number of particles, no new particles will be emitted\n"
            "                                                if the number of particles exceeds this parameter.\n"
            "\n"
            "Channel Options:\n"
            "      --height <double> (=75.0)               Height of the channel (m).\n"
            "      --radius <double> (=3.0)                Radius of the channel (m).\n"
            "      --n <double> (=800)                     Amount of \"volumes\" in the channel.\n"
            "      --globbc <enum> (=1)                    Global boundary condition:\n"
            "                                                1: Pressure gradient (usually negative).\n"
            "                                                2: Bulk velocity (m/s).\n"
            "      --globbv <double> (=-1)                 Global boundary value.\n"
            "                                              ! Meaning dependent on --globbv. Standard SI-units. !\n"
            "      --wallbc <enum> (=1)                    Boundary condition at the wall:\n"
            "                                                1: Velocity at the wall.\n"
            "                                                2: Velocity gradient at the wall.\n"
            "      --wallbv <double> (=0)                  Wall boundary value.\n"
            "                                              ! Meaning dependent on --wallbv. Standard SI-units. !\n"
            "      --mbounce <enum> (=1)                   Bounce condition for the particles/wall:\n"
            "                                                1: Stick to the wall.\n"
            "      --mloop <enum> (=2)                     Model used to get the velocity profile:\n"
            "                                                1: Constant viscosity.\n"
            "                                                2: Prantl mixing length.\n"
            "\n"
            "Fluid Options:\n"
            "      --flmu <double> (=1.8e-005)             Fluid Dynamic Viscocity (Pa s).\n"
            "      --flrho <double> (=1.0)                 Fluid Density (kg/m3).\n"
            "\n"
            "Particle Options:\n"
            "      --pdensity <double> (=1000.0)           Density of the particles (kg/m3).\n"
            "      --pdiameter <double> (=5.0E-5)          Diameter of the particles (m).\n"
            "\n"
            "Emitter Options:\n"
            "      --etype <enum> (=1)                     Emitter type:\n"
            "                                                1: Emit the grid specified with --dim once.\n"
            "                                                2: Emit continuously from the --dim grid.\n"
            "                                                3: Emit continuously and random from the --dim grid.\n"
            "      --dim <string> (=[-3:30:3,60:1:60])     X x Y grid of particles\n"
            "                                                Format: [start_x:steps_x:end_x,\n"
            "                                                         start_y:steps_y:end_y]\n"
            "      --rate <double> (=100.0)                Amount of particles emitted per second.\n"
            "      --initvel <string> (=[0,0])             Particle's initial velocity in m/s.\n"
            "\n"
            "Output Options:\n"
            "      --oformat <int> (=1)                    Output formats:\n"
            "                                                1: Byte\n"
            "                                                2: Text\n"
            "      --oinfo <int> (=0)                      Output types:\n"
            "                                                0: No writing to file.\n"
            "                                                1: Particle positions.\n"
            "                                                2: Velocity profile of the channel.\n"
            "      --oint <double> (=1.0)                  Write every <double> seconds.\n"
            "      --out <string> (=test.data)             The path to the output file.\n"
          );
}


// Parse commandline
void parse( int argc, char* argv[], ScrubberParam *param ) {

    using GetOpt::GetOpt_pp;
    using GetOpt::Option;
    using GetOpt::OptionPresent;

    GetOpt_pp ops(argc, argv);

    if ( ops >> OptionPresent( 'h', "help" ) )
    {
        show_help();
        exit( 1 );
    }

    // Temporary parse variables
    string s_edim;
    string s_initvel;
    double gravangle;

    // FIXME: Casts to integer for the enums.
    // Parse the cmdline
    ops >> Option( 'a', "duration",  param->duration, 600.0 )
        >> Option( 'a', "dtscale",   param->dtscale,  0.5 )
        >> Option( 'a', "errork",    param->errork,   1E-5 )
        >> Option( 'a', "relax",     param->relax,    0.9 )
        >> Option( 'a', "gravangle", gravangle,       0.0 )
        >> Option( 'a', "maxp",      param->maxparticles, 1000 );
        // Channel Options
    ops >> Option( 'a', "height",  param->channel.height,       75.0 )
        >> Option( 'a', "radius",  param->channel.radius,       3.0 )
        >> Option( 'a', "n",       param->channel.n,            800 )
        >> Option( 'a', "globbc",  param->channel.globbc,       (int) GBC_PRESSURE )
        >> Option( 'a', "globbv",  param->channel.globbv,       -0.01 )
        >> Option( 'a', "wallbc",  param->channel.wallbc,       (int) WBC_VELOCITY )
        >> Option( 'a', "wallbv",  param->channel.wallbv,       0.0 )
        >> Option( 'a', "mbounce", param->channel.bounce_model, (int) BOUNCE_STICK )
        >> Option( 'a', "mloop",   param->channel.loop_model,   (int) LM_PRANDTL );
        // Fluid Options
    ops >> Option( 'a', "flmu",  param->fl.mu,  1.8E-5 )
        >> Option( 'a', "flrho", param->fl.density, 1.0 );
        // Particle Options
    ops >> Option( 'a', "pdensity", param->p.density,  1000.0 )
        >> Option( 'a', "pradius",  param->p.radius,   1E-4 );
        // Emitter Options
    ops >> Option( 'a', "etype",   param->emitter.type, (int) EMITTER_ONCE )
        >> Option( 'a', "dim",     s_edim,              "[-3:30:3,60:1:60]" )
        >> Option( 'a', "rate",    param->emitter.rate, 100.0 )
        >> Option( 'a', "initvel", s_initvel,           "[0,0]" );
        // Output Options
    ops >> Option( 'a', "oformat", param->output.format,  (int) OUTPUT_BYTE )
        >> Option( 'a', "oinfo",   param->output.info,    (int) OUTPUT_NOTHING )
        >> Option( 'a', "oint",    param->output.interval, 1.0 )
        >> Option( 'a', "out",     param->output.path,     "test.data" );

    // Parse and write the temporary variables to the param struct
    param->gravity = 9.81 * Vector2d( sin(gravangle), -cos(gravangle) );

    // Extra channel stuff
    param->channel.dx = param->channel.radius*2 / param->channel.n;

    // Extra fluid stuff
    param->fl.nu = param->fl.mu / param->fl.density;

    // Read grid+delimiter+deltas for the emitter.
    readGridDelimiterDelta( s_edim, &param->emitter.grid, &param->emitter.delimiter,
                            &param->emitter.dx, &param->emitter.dy );

    // Initial velocity
    double init_vel_x, init_vel_y;
    sscanf( s_initvel.c_str(), "[%lf,%lf]", &init_vel_x, &init_vel_y );
    param->emitter.init_velocity = Vector2d( init_vel_x, init_vel_y );

    // Calculate the amount of particles in one grid.
    param->emitter.p_N = product( param->emitter.grid );

    // In case of the GridEmitters (1 and 2) maxparticles should be at least the size of one grid.
    if ( param->emitter.type == EMITTER_ONCE || param->emitter.type == EMITTER_GRID )
    {
        if ( param->maxparticles < param->emitter.p_N )
            param->maxparticles = param->emitter.p_N;
    }


    param->beta = param->p.density / param->fl.density;

    param->tau_p = param->p.density * 4 * param->p.radius * param->p.radius / (18 * param->fl.nu);
    param->tau_a = (param->beta + 0.5) / param->beta * param->tau_p;

    param->dt = param->dtscale * param->tau_p;

    // Can't output more data than we have
    if ( param->output.interval < param->dt )
    {
        printf( "Warning: output interval was smaller than dt, continuing with emitting every dt." );
        param->output.interval = param->dt;
    }
}

// Parse formatted strings
void readGridDelimiterDelta( const string &fstring, TGrid *grid, TDelimiter *delimiter,
                             double *dx, double *dy )
{
    double x1, x2, y1, y2;
    int X, Y;

    // Read the values into the variables.
    sscanf( fstring.c_str(), "[%lf:%d:%lf,%lf:%d:%lf]", &x1, &X, &x2, &y1, &Y, &y2 ); //e.g. [-4:30:4,0:1:0,4:1:4]"

    *delimiter = x1, x2,
                 y1, y2;
    *grid = X, Y;

    // Readability:
    const TDelimiter & _delimiter = *delimiter;
    const TGrid & _grid = *grid;

    // Set dx/dy/dz, and do some checks on them.
    *dx = ( _delimiter(0, 1) - _delimiter(0, 0) ) / ( _grid(0) - 1 );
    *dy = ( _delimiter(1, 1) - _delimiter(1, 0) ) / ( _grid(1) - 1 );

    if ( _grid(0) <= 1 )
        *dx = 0;

    if ( _grid(1) <= 1 )
        *dy = 0;
}

void printParam( const ScrubberParam &param )
{
    printf( "System Time (tau_p): %.5g\n", param.tau_p );
    printf( "System Time (tau_a): %.5g\n", param.tau_a );
    printf( "Timestep size (dt):  %.5g\n", param.dt );
    printf( "Amount of timesteps: %.5g\n", param.duration / param.dt );
}
