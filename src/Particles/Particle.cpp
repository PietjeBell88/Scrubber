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
#include "Particle.h"


// Constructor / Destructor
Particle::Particle()
{
    this->pos = Vector2d( 0, 0 );
    this->vel = Vector2d( 0, 0 );
    this->v_vel = Vector2d( 0, 0 );
    this->count_down = 0;
    this->gram_co2 = 0;
}

Particle::Particle( const Vector2d &pos, const Vector2d &vel )
{
    this->pos = pos;
    this->vel = vel;
    this->v_vel = Vector2d( 0, 0 );
    this->count_down = 0;
    this->gram_co2 = 0;
}


// Getters and Setters
const Vector2d &Particle::getPos() const
{
    return pos;
}

void Particle::setPos( const Vector2d &pos )
{
    this->pos = pos;
}

const Vector2d &Particle::getVel() const
{
    return vel;
}

void Particle::setVel( const Vector2d &vel )
{
    this->vel = vel;
}

const Vector2d &Particle::getSurroundingVel() const
{
    return v_vel;
}

void Particle::setSurroundingVel( const Vector2d &v_vel )
{
    this->v_vel = v_vel;
}

double Particle::getCountDown() const
{
    return count_down;
}

void Particle::setCountDown( double count_down )
{
    this->count_down = count_down;
}

double Particle::getGramCO2() const
{
    return gram_co2;
}

void Particle::setGramCO2( double gram_co2 )
{
    this->gram_co2 = gram_co2;
}
