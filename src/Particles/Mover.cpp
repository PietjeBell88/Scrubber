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
#include "Channel/Channel.h"


// Constructor / Destructor
Mover::Mover( const ScrubberParam &param, Channel *channel )
{
    this->gravity = param.gravity;
    this->dt = param.dt;
    this->beta = param.beta;
    this->tau_a = param.tau_a;

    // FIXME: Cast to enum from integer (thanks to parameter parser sucking).
    this->bounce_model = (BounceModel) param.channel.bounce_model;

    this->channel = channel;
    this->emitter = emitter;
}


// Private Methods
void Mover::bounceWall( const Vector2d &old_pos, Vector2d *new_pos, Vector2d *new_vel )
{
    switch( bounce_model ) {
        case BOUNCE_STICK:
            // Already handled in doMove()
            break;
    }
}


// Public Methods
void Mover::doMove( ParticleArray *particles )
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
            case P_OUTSIDE_BOTTOM:
                particles->remove( p );
                break;
            case P_OUTSIDE_SIDE:
                if ( bounce_model == BOUNCE_STICK )
                {
                    particles->remove( p );
                    break;
                }
                else
                    bounceWall( p_pos, &new_pos, &new_vel );
            case P_INSIDE:
                // Give the particle his new position.
                particle.setPos( new_pos );
                particle.setVel( new_vel );

                // Write the particle back to the array (maybe, and hopefully, it will be written
                // directly to the array instead of making a temporary object like "particle" is.
                particles->setParticle( p, particle );
                break;
        }
    }
}
