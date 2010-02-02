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

#include "InOut/InOut.h"
#include "InOut/ByteInOut.h"
#include "InOut/TextInOut.h"

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
    fflush( stdout );
}

int main( int argc, char* argv[] )
{
    // Parameters
    ScrubberParam param;

    // Parse the parameters
    parse( argc, argv, &param );
    printParam( param );

    ScalarField u;

    if ( param.input.format > INOUT_NOIMPORT )
    {
        // Making the input reader and read the velocity profile
        InOut * input;

        switch ( param.input.format ) {
            case INOUT_BYTE:
                input = new ByteInOut( param );
                break;
            case INOUT_TEXT:
                input = new TextInOut( param );
                break;
            default:
                cout << "Unknown input type.";
                break;
        }

        input->readProfile( &param, &u );

        delete input;
    }

    // Making the output writer
    InOut * output;

    switch ( param.output.format ) {
        case INOUT_BYTE:
            output = new ByteInOut( param );
            break;
        case INOUT_TEXT:
            output = new TextInOut( param );
            break;
        default:
            cout << "Unknown output type.";
            break;
    }

    // Making the channel
    Channel *channel = new Channel( param );

    if ( param.input.format == INOUT_NOIMPORT )
        channel->init();
    else
        channel->init( u );

    if ( param.output.info == OUTPUT_VELFIELD )
    {
        output->writeScalarField( channel->getVelocityField() );
        delete output;
        delete channel;
        return 0;
    }

    // Checking if the particles are heavy enough.
    // FIXME: Should be done in the mover?
    double max_velocity = max( channel->getVelocityField() );
    double speed_p = 1 / param.tau_a * max_velocity + (param.beta - 1) / (param.beta + 0.5) * param.gravity(1);
    if ( speed_p > 0 )
        printf( "Warning: Some particles might not be heavy enough to fall all the way down.\n" );

    // Making the Emitter
    Emitter *emitter;

    switch ( param.emitter.type ) {
        case EMITTER_ONCE:
            emitter = new GridOnceEmitter( param, channel );
            break;
        case EMITTER_GRID:
            emitter = new GridEmitter( param, channel );
            break;
        case EMITTER_RANDOM:
            emitter = new RandomEmitter( param, channel );
            break;
        default:
            cout << "Unknown Emitter type";
            exit( 1 );
    }

    // Making the particle mover
    Mover * mover;

    mover = new Mover( param, channel );

    // Making a struct to keep track of statistics
    StatsStruct stats;

    // Allocating memory for the array that holds the particles
    ParticleArray particles( param.maxparticles );

    // Emit the particles
    emitter->init( &particles );

    double time = 0;
    output->writeToFile( time, particles );
    double time_next_output = param.output.interval;

    while ( time <= param.duration )
    {
        writeProgress( (int) (100 * time / param.duration) );

        // Move the particles
        mover->doMove( &particles, &stats );

        emitter->update( time, &particles );

        // Write to file
        if ( time >= time_next_output )
        {
            output->writeToFile( time, particles );
            time_next_output += param.output.interval;
        }

        time += param.dt;

        // Break when there are no more particles to plot.
        if ( param.emitter.type == EMITTER_ONCE
             && particles.getLength() == 0 )
        {
            break;
        }
    }

    printf( "Done after %.5g seconds (%d%%).\n", time, (int) (100 * time / param.duration) );

    int total_out;

    if ( param.channel.bounce_model == BOUNCE_STICK )
        total_out = stats.p_top + stats.p_bottom + stats.p_wall;
    else
        total_out = stats.p_top + stats.p_bottom;

    printf( "In total %d (* %.5g) particles left the box:\n", total_out, param.p.clustersize );
    printf( "  - Top:    %d (%.5g%%)\n", stats.p_top, 100 * (double) stats.p_top / total_out );
    printf( "  - Bottom: %d (%.5g%%)\n", stats.p_bottom, 100 * (double) stats.p_bottom / total_out );
    if ( param.channel.bounce_model == BOUNCE_STICK )
        printf( "  - Wall:   %d (%.5g%%)\n", stats.p_wall, 100 * (double) stats.p_wall / total_out );
    else
        printf( "%d times particles bounced off the wall.\n", stats.p_wall );

    // In liters:
    const double used_mea = param.p.clustersize * param.p.mole_mea_total * param.mea.mole_mass * total_out / param.mea.density;
    const double used_solvent = param.p.clustersize * param.p.mole_solvent * param.p.mole_mass * total_out / param.p.density;

    // Multiply the captured co2 by the clustersize
    stats.captured_co2 = param.p.clustersize * stats.captured_co2;

    printf( "Captured CO2: %.5g gram\n", stats.captured_co2 );
    printf( "Used MEA: %.5g L\n", used_mea );
    printf( "Used solvent: %.5g L\n", used_solvent );
    printf( "CO2 / MEA: %.5g g/L\n", stats.captured_co2 / used_mea );
    printf( "CO2 / total: %.5g g/L\n", stats.captured_co2 / (used_mea + used_solvent) );

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
            "      --dtscale <double> (=0.5)               dt = dtscale * min( tau_p, tau_m ).\n"
            "      --errork <double> (=1E-5)               The error threshold for the steady velocity profile.\n"
            "      --relax <double> (=0.9)                 Relaxation for prandtl mixing length. 0 = none, 0.99 = a lot.\n"
            "      --gravangle <double> (=0.0)             Angle of gravity with the negative z-axis.\n"
            "      --maxp <int> (=1000)                    Maximum number of particles, no new particles will be emitted\n"
            "                                                if the number of particles exceeds this parameter.\n"
            "\n"
            "Channel Options:\n"
            "      --height <double> (=75.0)               Height of the channel (m).\n"
            "      --radius <double> (=3.0)                Radius of the channel (m).\n"
            "      --conc_b <double> (=0.05)               Volume fraction of CO2 at the bottom.\n"
            "      --conc_t <double> (=0.01)               Volume fraction of CO2 at the top.\n"
            "      --n <double> (=800)                     Amount of \"volumes\" in the channel.\n"
            "      --globbc <enum> (=2)                    Global boundary condition:\n"
            "                                                1: Pressure gradient (usually negative).\n"
            "                                                2: Bulk velocity (m/s).\n"
            "      --globbv <double> (=3.0)                Global boundary value.\n"
            "                                              ! Meaning dependent on --globbc. Standard SI-units. !\n"
            "      --wallbc <enum> (=1)                    Boundary condition at the wall:\n"
            "                                                1: Velocity at the wall.\n"
            "                                                2: Velocity gradient at the wall.\n"
            "      --wallbv <double> (=0.0)                Wall boundary value.\n"
            "                                              ! Meaning dependent on --wallbc. Standard SI-units. !\n"
            "      --mbounce <enum> (=1)                   Bounce condition for the particles/wall:\n"
            "                                                1: Stick to the wall.\n"
            "                                                2: Sliding collision:\n"
            "      --cor <double> (=1.0)                         - Coefficient of restitution.\n"
            "      --friction <double> (=0.0)                    - Friction coefficient of the wall.\n"
            "      --mloop <enum> (=3)                     Model used to get the velocity profile:\n"
            "                                                1: Constant viscosity.\n"
            "                                                2: Prandtl mixing length.\n"
            "                                                3: Van Driest model.\n"
            "      --mturb <enum> (=1)                     Model to use for eddies / velocity pertubations.\n"
            "                                                0: None (no turbulence modelling).\n"
            "                                                1: Discrete eddy model.\n"
            "                                                2: Langevin model.\n"
            "\n"
            "Fluid Options:\n"
            "      --flmu <double> (=1.8e-005)             Fluid Dynamic Viscocity (Pa s).\n"
            "      --flrho <double> (=1.0)                 Fluid Density (kg/m3).\n"
            "\n"
            "Particle Options:\n"
            "      --pdensity <double> (=1000.0)           Density of the particles (kg/m3).\n"
            "      --pradius <double> (=3E-4)              Diameter of the particles (m).\n"
            "      --frac_mea <double> (=0.40)             Mass fraction of MEA.\n"
            "      --cluster <double> (=1.0)               Particle cluster size.\n"
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
            "Input Options:\n"
            "      --profile <string> (=\"\")                Path to profile data (Leave empty to calculate).\n"
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
        >> Option( 'a', "conc_b",  param->channel.conc_b,       0.05 )
        >> Option( 'a', "conc_t",  param->channel.conc_t,       0.01 )
        >> Option( 'a', "n",       param->channel.n,            800 )
        >> Option( 'a', "globbc",  param->channel.globbc,       (int) GBC_BULK_VEL )
        >> Option( 'a', "globbv",  param->channel.globbv,       3.0 )
        >> Option( 'a', "wallbc",  param->channel.wallbc,       (int) WBC_VELOCITY )
        >> Option( 'a', "wallbv",  param->channel.wallbv,       0.0 )
        >> Option( 'a', "mbounce", param->channel.bounce_model, (int) BOUNCE_STICK )
        >> Option( 'a', "cor",     param->channel.c_restitution, 1.0 )
        >> Option( 'a', "friction", param->channel.c_friction,  0.0 )
        >> Option( 'a', "mloop",   param->channel.loop_model,   (int) LM_VAN_DRIEST )
        >> Option( 'a', "mturb",   param->channel.turb_model,   (int) TURB_DISCRETE_EDDY );
        // Fluid Options
    ops >> Option( 'a', "flmu",  param->fl.mu,  1.8E-5 )
        >> Option( 'a', "flrho", param->fl.density, 1.0 );
        // Particle Options
    ops >> Option( 'a', "pdensity", param->p.density,  1000.0 )
        >> Option( 'a', "pradius",  param->p.radius,   3E-4 )
        >> Option( 'a', "frac_mea", param->p.mass_frac_mea, 0.40 )
        >> Option( 'a', "cluster",  param->p.clustersize, 1.0 );
        // Emitter Options
    ops >> Option( 'a', "etype",   param->emitter.type, (int) EMITTER_ONCE )
        >> Option( 'a', "dim",     s_edim,              "[-3:30:3,60:1:60]" )
        >> Option( 'a', "rate",    param->emitter.rate, 100.0 )
        >> Option( 'a', "initvel", s_initvel,           "[0,0]" );
        // Input Options
    ops >> Option( 'a', "profile", param->input.path,   "" );
        // Output Options
    ops >> Option( 'a', "oformat", param->output.format,  (int) INOUT_BYTE )
        >> Option( 'a', "oinfo",   param->output.info,    (int) OUTPUT_NOTHING )
        >> Option( 'a', "oint",    param->output.interval, 1.0 )
        >> Option( 'a', "out",     param->output.path,     "test.data" );

    // Parse and write the temporary variables to the param struct
    param->gravity = 9.81 * Vector2d( sin(gravangle), -cos(gravangle) );

    // Extra channel stuff
    param->channel.dx = param->channel.radius*2 / param->channel.n;

    // Extra fluid stuff
    param->fl.nu = param->fl.mu / param->fl.density;

    // CO2 properties
    param->co2.mole_mass = 44.01;
    param->co2.diffusivity = 0.14E-4;
    param->co2.density = 1.98;

    // MEA properties
    param->mea.mole_mass = 61.0;
    param->mea.density = 1E3;

    // Solvent properties
    param->p.mole_mass = 18.0;

    // Extra/redundant particle stuff
    param->p.diameter = 2 * param->p.radius;
    param->p.volume = (4.0 / 3.0) * PI * pow3( param->p.radius );
    param->p.mass = param->p.density * param->p.volume;
    param->p.mole_mea_total = param->p.mass_frac_mea * param->p.mass * 1000.0 / param->mea.mole_mass ;
    param->p.mole_solvent = (1.0 - param->p.mass_frac_mea) * param->p.mass * 1000.0 / param->p.mole_mass;

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

    param->tau_m = param->p.density * pow2( param->p.diameter ) /
                   ( 12.0 * param->co2.density * param->co2.diffusivity);

    // Check both the particle acceleration time and the mass transfer time for timestep size
    param->dt = param->dtscale * min( param->tau_p, param->tau_m );

    // Check for possible input
    if ( param->input.path == "" )
        param->input.format = INOUT_NOIMPORT;
    else
    {
        FILE *f;
        if( (f = fopen( param->input.path.c_str(), "r" )) == NULL )
        {
           printf("Problem opening profile file %s.\n", param->input.path.c_str());
           exit( 1 );
        }

        // Read the file type
        int nread = fread( &param->input.format, 4, 1, f );
        fclose( f );
    }

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
    printf( "Mass Transfer Time (tau_m): %.5g\n", param.tau_m );
    printf( "Timestep size (dt):  %.5g\n", param.dt );
    printf( "Amount of timesteps: %.5g\n\n", param.duration / param.dt );
}
