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
#include "Typedefs.h"


// Forward Declarations
class Emitter;
class Channel;
class ParticleArray;


// using
using std::string;


// Enums
enum PosBox
{
    P_INSIDE,
    P_OUTSIDE_SIDE,
    P_OUTSIDE_TOP,
    P_OUTSIDE_BOTTOM
};

enum BounceModel
{
    BOUNCE_STICK = 1,
};

enum LoopModel
{
    LM_SIMPLE = 1,
    LM_PRANDTL
};

enum TurbModel
{
    TURB_NONE,
    TURB_DISCRETE_EDDY,
    TURB_LANGEVIN
};

enum OutputInfo
{
    OUTPUT_NOTHING,
    OUTPUT_POSITIONS,
    OUTPUT_VELFIELD
};

enum InOutFormat
{
    INOUT_NOIMPORT,
    INOUT_BYTE,
    INOUT_TEXT
};

enum EmitterType
{
    EMITTER_ONCE = 1,
    EMITTER_GRID,
    EMITTER_RANDOM
};

enum GlobalBC
{
    GBC_PRESSURE = 1,
    GBC_BULK_VEL
};

enum WallBC
{
    WBC_VELOCITY = 1,
    WBC_VEL_GRADIENT,
};


// Global Settings
// FIXME: Enums had to be integers, otherwise getopt_pp wouldn't parse the parameters.
struct ScrubberParam
{
    double duration;  /// Duration of the computation in seconds.
    double dtscale;   /// The multiplication factor to get from tau_p to dt.
    double dt;        /// Stepsize of the time

    double tau_p;     /// System time
    double tau_a;     /// System time compensated for density ratio

    double errork;    /// Maximum error during calculations
    double relax;     /// Relaxation parameter. 0 = no relaxation, 0.99 = a lot.

    Vector2d gravity; /// Gravity vector

    int maxparticles; /// Max number of particles that can be emitted.

    // Channel specific parameters
    struct channel
    {
        double height;         /// Height of the channel (m)
        double radius;         /// Radius of the channel (m)

        int n;                 /// Amount of volumes at one height in the channel.
        double dx;             /// Stepsizes of the grid

        int wallbc;       /// <enum> Boundary Condition at the wall.
        double wallbv;    /// Value of boundary condition.
        int globbc;       /// <enum> Global boundary condition.
        double globbv;    /// Value of global boundary condition.

        int bounce_model; /// <enum> Sticky walls etc.
        int loop_model;   /// <enum> Model used to get fluid velocity profile in the channel.
        int turb_model;   /// <enum> Indicates what model to use for generation of eddies/pertubations.
    } channel;

    // Fluid properties
    struct fluid
    {
        double mu;       /// Fluid dynamic viscosity (Pa s)
        double nu;       /// Fluid kinematic viscosity (m^2/s)
        double density;  /// Fluid density (kg/m^3)
    } fl;

    // Global particle properties (the same for all particles)
    struct particle
    {
        double density;  /// Particle density (kg/m^3)
        double radius;   /// Particle radius (m)
    } p;

    // Emitter properites
    struct emitter
    {
        int type;             /// <enum> Emitter type.
        TGrid grid;           /// X x Y grid of particles
        TDelimiter delimiter; /// The edges of the box.
        double dx, dy;        /// The stepsizes.

        int p_N;                /// Amount of particles per grid.
        double rate;            /// Emitting rate.
        Vector2d init_velocity; /// Initial velocity the particles are emitted with.
    } emitter;

    // Input specific parameters
    struct input
    {
        int format;   /// <enum> Input type.
        string path;  /// Path to profile datafile.
    } input;

    // Output specific parameters
    struct output
    {
        int format;       /// <enum> Output type.
        int info;         /// <enum> Information to output (i.e. positions/trajectories/concentration/velocity field)
        double interval;  /// Interval in which to output (every <double> seconds)
        string path;      /// Path to datafile
    } output;

    // Calculated parameters
    double beta;  /// Ratio between fluid and particle density
};


// Functions
inline void writeProgress( int perc );

void show_help();

void parse( int argc, char* argv[], ScrubberParam *param );

void readGridDelimiterDelta( const string &fstring, TGrid *grid, TDelimiter *delimiter,
                             double *dx, double *dy );

void printParam( const ScrubberParam &param );
