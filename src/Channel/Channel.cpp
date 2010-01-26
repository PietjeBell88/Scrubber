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
#include "Channel.h"

#include "MTRand.h"

#include "CPModel.h"


// Constructor / Destructor
Channel::Channel( const ScrubberParam &param )
{
    this->height = param.channel.height;
    this->radius = param.channel.radius;

    this->n = param.channel.n;
    this->dx = param.channel.dx;

    this->turb_model = (TurbModel) param.channel.turb_model;

    this->cpmodel = new CPModel( param );

    // n volumes --> n+1 points. There's half a volume outside the channel,
    // because the velocity is defined at the edges, whereas the viscosity is
    // defined at the center.
    u.resize( n+2 );
    u = 0;
}

Channel::~Channel() {}


// Private methods
double Channel::interpolate2d( const Vector2d &pos )
{
    // Position should be located in the channel
    assert( abs( pos(0) ) <= radius );

    // Start by finding the lower index values of the box surrounding the particle.
    // Remember that (inherent to the finite volume method) velocities are defined at edges of a volume.
    int i = static_cast<int> (floor( (pos(0) + radius + 0.5 * dx) / dx ));

    /*
     * Calculate the weighting factors
     * Note: 0 <= x < 1
     * Modulo needs to be done on positive values.
     */
    double x = fmod(pos(0) + radius + 0.5 * dx, dx) / dx;

    //Do a weighted addition
    return u(i) * (1 - x) + u(i + 1) * x;
}

double Channel::dudy( const Vector2d &pos )
{
    // Start by finding the lower index values of the box surrounding the particle.
    int i = static_cast<int> (floor( (pos(0) + radius + 0.5 * dx) / dx ));

    //Do a weighted addition
    return (u(i+1) - u(i))/dx;
}


// Public methods
void Channel::init()
{
    u = cpmodel->init( u );
}

void Channel::init( const ScalarField &u )
{
    this->u = u;
}

PosBox Channel::outsideBox( const Vector2d &pos )
{
    if ( pos(1) >= height )
        // Left the channel at the top.
        return P_OUTSIDE_TOP;

    else if ( pos(1) <= 0 )
        // Left the channel at the bottom.
        return P_OUTSIDE_BOTTOM;

    else if ( abs( pos(0) ) >= radius )
        // Left the box on the sides.
        return P_OUTSIDE_SIDE;

    else
        // Still in the box.
        return P_INSIDE;
}

void Channel::velocityAt( const Vector2d &pos, const Vector2d &vel, Vector2d *v_vel, double *count_down )
{
    Vector2d mean_velocity = Vector2d( 0, interpolate2d( pos ) );

    if ( turb_model == TURB_NONE )
        *v_vel = mean_velocity;

    else if ( turb_model == TURB_DISCRETE_EDDY )
    {
        MTRand myran;

        // Discrete-eddy model
        // readability
        const double C_T = 0.3;

        const double u_acc = cpmodel->prandtlLength( pos(0) ) * abs( dudy( pos ) );
        const double sigma = abs( u_acc );

        const Vector2d new_velocity( myran.randNorm( mean_velocity(0), sigma ), myran.randNorm( mean_velocity(1), sigma ) );

        const double T_eddy = C_T / abs( dudy( pos ) );
        const double L_eddy = T_eddy * sigma;
        // FIXME: Check if norm is always positive.
        const double T_res = L_eddy / blitz::norm( vel - new_velocity );

        *count_down = min( T_res, T_eddy );
        *v_vel = new_velocity;
    }

    // FIXME: Checking should be done in parsing of commandline arguments.
    else
        printf( "Unkown turb_model [%d], stopping...\n", turb_model );
}

const ScalarField &Channel::getVelocityField() const
{
    return u;
}
