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
#include "Scrubber.h"


// Forward Declarations
class ParticleArray;
class Channel;
class Particle;


/**
 * Moves the particles.
 * Can move the particles based on a equation of motion and do checks on them
 * to see if they're still in the channel. Also has various methods of bouncing
 * of walls.
 */
class Mover {
private:
    Vector2d gravity;
    double dt;
    double beta;

    double tau_a;
    double pdiameter;
    double mole_mea_total;
    double mole_solvent;

    double co2_mole_mass;
    double co2_diffusivity;
    double co2_density;

    double nu;

    double c_restitution;
    double c_friction;

    double radius;

    BounceModel bounce_model;

    Channel *channel;

    /**
     * Bounces particles off the wall based on different models. Changes position and velocity.
     * @param old_pos  Old position of the particle.
     * @param new_pos  New position of the particle.
     * @param new_vel  Velocity of the particle.
     */
    void bounceWall( const Vector2d &old_pos, Vector2d *new_pos, Vector2d *new_vel );

    /**
     * Calculates the new amount (in gram) of totally absorbed CO2 in the particle.
     * @param p  The particle with its properties.
     * @return   The new total amount of CO2 in the particle.
     */
    double newGramCO2( const Particle &p );

public:
    /**
     * Constructor.
     * @param param    Struct of parameters.
     * @param channel  The channel with continuous phase.
     * @param emitter  Particle emitter/resetter.
     */
    Mover( const ScrubberParam &param, Channel *channel );

    /**
     * Moves the particles and does checks on them.
     * @param particles  The array of particles which will be checked.
     * @param stats      Keeps track of statistics.
     * @see              bounceWall()
     */
    void doMove( ParticleArray *particles, StatsStruct *stats );
};
