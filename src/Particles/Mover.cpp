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
#include "Mover.h"

#include "ParticleArray.h"
#include "Particle.h"

#include "Channel/Channel.h"


// Constructor / Destructor
Mover::Mover( const ScrubberParam &param, Channel *channel )
{
    this->gravity = param.gravity;
    this->dt = param.dt;
    this->beta = param.beta;

    this->tau_a = param.tau_a;
    this->pdiameter = param.p.diameter;
    this->mole_mea_total = param.p.mole_mea_total;
    this->mole_solvent = param.p.mole_solvent;

    this->co2_mole_mass = param.co2.mole_mass;
    this->co2_diffusivity = param.co2.diffusivity;
    this->co2_density = param.co2.density;

    this->nu = param.fl.nu;

    this->c_restitution = param.channel.c_restitution;
    this->c_friction = param.channel.c_friction;

    this->radius = param.channel.radius;

    // FIXME: Cast to enum from integer (thanks to parameter parser sucking).
    this->bounce_model = (BounceModel) param.channel.bounce_model;

    this->channel = channel;
    this->emitter = emitter;
}


// Private Methods
void Mover::bounceWall( const Vector2d &old_pos, Vector2d *new_pos, Vector2d *vel )
{
    switch( bounce_model ) {
        case BOUNCE_STICK:
            // Already handled in doMove()
            break;
        case BOUNCE_SLICOLL:
            const Vector2d old_vel = *vel;

            // Time it took the particle to get to the wall from its old position
            const double time_before = ( radius - abs( old_pos(0) ) ) / abs( old_vel(0) );

            // Position of particle when it hit the wall
            const Vector2d wall_pos( ( old_pos(0) < 0 ) ? -radius : radius,
                                     old_pos(1) + time_before * old_vel(1) );

            // New velocity
            *vel = Vector2d( - c_restitution * old_vel(0),
                             old_vel(1) + c_friction * ( 1 + c_restitution ) * old_vel(0) );

            // New position
            *new_pos = wall_pos + (*vel) * ( dt - time_before );
            break;
    }
}

double Mover::newGramCO2( const Particle &p )
{
    // Readability
    const double p_gram_co2 = p.getGramCO2();
    const double p_mole_co2 = p_gram_co2 / co2_mole_mass;
    const Vector2d pos = p.getPos();
    const Vector2d vel = p.getVel();

    // Get the concentration at the heigth of the particle
    double fl_mass_frac_co2 = channel->massFracAt( pos );

    // Calculate some dimensionless numbers
    double Re_p = blitz::norm( vel ) * pdiameter / nu ;
    static double Sc = nu / co2_diffusivity;
    double Sh = 2.0 + 0.66 * sqrt( Re_p ) * pow( Sc, 1.0 / 3 );

    // Calculating the fraction of free MEA in the particle, and use it as a "correction" factor
    double correction_factor = ( mole_mea_total - 2.0 * p_mole_co2 ) / ( mole_mea_total + mole_solvent );

    // Break when correction factor is negative, because losing CO2 is endothermic
    if ( correction_factor <= 0 )
        return p_gram_co2;

    // Calculating the mass (kg) of CO2 added in a timestep
    double dm = Sh * PI * pdiameter * co2_density * co2_diffusivity * fl_mass_frac_co2 * dt * correction_factor;

    // Return the new total amount of co2 in the particle in gram
    return p_gram_co2 + dm * 1000.0;
}


// Public Methods
void Mover::doMove( ParticleArray *particles, StatsStruct *stats )
{
    /*
     * See Formula 11 in M.F. Cargnelutti and Portela's "Influence of the resuspension on
     * the particle sedimentation in wall-bounded turbulent flows") for the equation of motion.
     */
    for ( int p = 0; p < particles->getLength(); p++ )
    {
        // Get the particle.
        Particle particle = particles->getParticle( p );

        // Readability for rhs.
        const Vector2d & p_pos = particle.getPos();
        const Vector2d & p_vel = particle.getVel();

        // Get the velocity of the fluid surrounding the particle
        Vector2d v_vel = particle.getSurroundingVel();

        // Get countdown
        double count_down = particle.getCountDown();
        count_down = count_down - dt;

        if ( count_down <= 0 )
        {
            // Get a new surrounding velocity
            channel->velocityAt( p_pos, p_vel, &v_vel, &count_down );
            particle.setSurroundingVel( v_vel );
        }

        particle.setCountDown( count_down );

        // Particle equation of motion.
        const Vector2d dv = (1 / tau_a * (v_vel - p_vel) + (beta - 1) / (beta + 0.5) * gravity ) * dt;

        // Set the new particle velocity and calculate the new position
        Vector2d new_vel = p_vel + dv;
        Vector2d new_pos = p_pos + new_vel * dt;

        // Check if the particle is outside the channel
        switch ( channel->outsideBox( new_pos ) ) {
            case P_OUTSIDE_TOP:
                stats->p_top++;
                // Remove the particle and update co2 stats
                stats->captured_co2 += particles->remove( p ).getGramCO2();
                break;
            case P_OUTSIDE_BOTTOM:
                stats->p_bottom++;
                // Remove the particle and update co2 stats
                stats->captured_co2 += particles->remove( p ).getGramCO2();
                break;
            case P_OUTSIDE_SIDE:
                stats->p_wall++;
                if ( bounce_model == BOUNCE_STICK )
                {
                    // Remove the particle and update co2 stats
                    stats->captured_co2 += particles->remove( p ).getGramCO2();
                    break;
                }
                else
                    bounceWall( p_pos, &new_pos, &new_vel );
            case P_INSIDE:
                // Give the particle his new position.
                particle.setPos( new_pos );
                particle.setVel( new_vel );

                particle.setGramCO2( newGramCO2( particle ) );

                // Write the particle back to the array (maybe, and hopefully, it will be written
                // directly to the array instead of making a temporary object like "particle" is.
                particles->setParticle( p, particle );
                break;
        }
    }
}
