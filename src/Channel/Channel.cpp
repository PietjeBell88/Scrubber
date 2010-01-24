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

#include "CPModel.h"


// Constructor / Destructor
Channel::Channel( const ScrubberParam &param )
{
    this->height = param.channel.height;
    this->radius = param.channel.radius;

    this->n = param.channel.n;
    this->dx = param.channel.dx;

    this->cpmodel = new CPModel( param );

    // n volumes --> n+1 points. There's half a volume outside the channel,
    // because the velocity is defined at the edges, whereas the viscosity is
    // defined at the center.
    u.resize( n+2 );
    u = 0;
}

Channel::~Channel() {}


// Private methods
Vector2d Channel::interpolate2d( const Vector2d &pos )
{
    // Start by finding the lower index values of the box surrounding the particle.
    int i = static_cast<int> (floor( (pos(0) + radius) / dx ));

    /*
     * Calculate the weighting factors
     * Note: 0 <= x < 1
     * Modulo needs to be done on positive values.
     */
    double x = fmod(pos(0) + radius, dx) / dx;

    //Do a weighted addition
    return Vector2d( 0, u(i) * (1 - x) + u(i + 1) * x );
}


// Public methods
void Channel::init()
{
    u = cpmodel->init( u );
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

Vector2d Channel::velocityAt( const Vector2d &pos )
{
    return interpolate2d( pos );
}

const ScalarField &Channel::getVelocityField() const
{
    return u;
}
