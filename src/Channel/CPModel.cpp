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
#include "CPModel.h"


// Constants
const double CPModel::lambda = 0.09;
const double CPModel::kappa = 0.41;


// Constructor / Destructor
CPModel::CPModel( const ScrubberParam &param )
{
    // Readability
    this->mu = param.fl.mu;
    this->rho = param.fl.density;
    this->nu = param.fl.nu;

    this->dy = param.channel.dx;  /// <- DON'T GET CONFUSED
    this->diameter = 2*param.channel.radius;
    this->delta = param.channel.radius;
    this->n = param.channel.n;

    // FIXME: Casts to enums from integer (thanks to parameter parser sucking).
    this->loop_model = (LoopModel) param.channel.loop_model;
    this->errork = param.errork;
    this->relax = param.relax;

    this->globbc = (GlobalBC) param.channel.globbc;
    this->globbv = param.channel.globbv;
    this->wallbc = (WallBC) param.channel.wallbc;
    this->wallbv = param.channel.wallbv;
}

CPModel::~CPModel() {}


// Private methods
ScalarField CPModel::velocityProfile( ScalarField u )
{
    // FIXME: Check for invalid loop_model shouldn't be done here, but when parsing commandline arguments.
    // Calculate the velocity profile based on various loop models.
    switch ( loop_model ) {
        case LM_SIMPLE:
            u = loopSimple( u );
            break;
        case LM_PRANDTL:
            u = loopPrandtl( u );
            break;
        case LM_VAN_DRIEST:
            u = loopVanDriest( u );
            break;
        default:
            printf( "Unkown loop_model [%d], stopping...\n", loop_model );
            break;
    }
    return u;
}

ScalarField CPModel::loopSimple( ScalarField u )
{
    ScalarField unew( u.shape() );
    unew = 0;

    double error = 10000;

    // Loop while the error is too big.
    while ( error > errork )
    {
        // Walk over all volumes
#pragma omp parallel for
        for ( int i = 1; i <= n; i++ )
            unew(i) = (-pg * pow2( dy ) + mu * u(i+1) + mu * u(i-1) )/(2*mu);

        // Set the ghost point
        setGhost( &unew );

        // Get the new error
        error = froNorm( unew, u );

        u = unew;
    }
    return u;
}

ScalarField CPModel::loopPrandtl( ScalarField u )
{
    ScalarField unew( u.shape() );
    unew = 0;

    double error = 10000;

    // Loop while the error is too big.
    while ( error > errork )
    {
#pragma omp parallel for
        for ( int i = 1; i <= n ; i++ )
        {
            // Readability. Calculate the prandtl mixing length and subsequently the
            // viscosity at the top and bottom points.
            const double l_b = prandtlLength( y_mu(i-1) );
            const double l_t = prandtlLength( y_mu(i) );

            const double mu_b = mu + pow2( l_b ) * rho * abs( dudy( u, i-1 ) );
            const double mu_t = mu + pow2( l_t ) * rho * abs( dudy( u, i ) );

            unew(i) = (-pg * pow2( dy ) + mu_t * u(i+1) + mu_b * u(i-1)) /
                      (mu_t + mu_b) *
                      (1-relax) + u(i) * relax;
        }

        // Set the ghost point
        setGhost( &unew );

       // Get the new error
        error = froNorm( unew, u );

        // Write the new values to u
        u = unew;
    }
    return u;
}

ScalarField CPModel::loopVanDriest( ScalarField u )
{
    ScalarField unew( u.shape() );
    unew = 0;

    double error = 10000;

    // Loop while the error is too big.
    while ( error > errork )
    {
#pragma omp parallel for
        for ( int i = 1; i <= n ; i++ )
        {
            // Readability. Calculate the prandtl mixing length and subsequently the
            // viscosity at the top and bottom points.
            const double l_b = prandtlLength( y_mu(i-1) );
            const double l_t = prandtlLength( y_mu(i) );

            const double y_a_b = y_mu(i-1) * abs( l_b ) * abs( dudy( u, i-1 ) ) / (25 * nu);
            const double y_a_t = y_mu(i) * abs( l_t ) * abs( dudy( u, i ) ) / (25 * nu);

            const double l_b_vd = l_b * (1 - exp( -y_a_b ) );
            const double l_t_vd = l_t * (1 - exp( -y_a_t ) );

            const double mu_b = mu + pow2( l_b_vd ) * rho * abs( dudy( u, i-1 ) );
            const double mu_t = mu + pow2( l_t_vd ) * rho * abs( dudy( u, i ) );

            unew(i) = (-pg * pow2( dy ) + mu_t * u(i+1) + mu_b * u(i-1)) /
                      (mu_t + mu_b) *
                      (1-relax) + u(i) * relax;
        }

        // Set the ghost point
        setGhost( &unew );

       // Get the new error
        error = froNorm( unew, u );

        // Write the new values to u
        u = unew;
    }
    return u;
}

double CPModel::froNorm( const ScalarField &new_field, const ScalarField &old_field )
{
    double sum = 0;
    int i;
    for ( i = 0; i < new_field.shape()(0); i++ )
    {
        const double & el_new = new_field(i);
        const double & el_old = old_field(i);
        if ( el_new > 0)
            sum += pow2( (el_new - el_old)/el_new );
    }
    if ( sum == 0 )
        return 1.0;
    else
        return pow( sum, 0.5 );
}

void CPModel::setGhost( ScalarField *u )
{
    int last = u->shape()(0)-1;
    assert( n+1 == last );

    // Readability
    const ScalarField & _u = *u;

    switch ( wallbc )
    {
        case WBC_VELOCITY:
            (*u)(0)    = 2 * wallbv - _u(1);
            (*u)(last) = 2 * wallbv - _u(last-1);
            break;
        case WBC_VEL_GRADIENT:
            (*u)(last) = wallbv * dy + _u(last+1);
            (*u)(0)    = wallbv * dy - _u(1);
            break;
    }
}


// Public Methods
ScalarField CPModel::init( ScalarField u )
{
    // Allocate the y vector and fill it with values.
    y_mu.resize( n+1 );

    for (int i = 0; i < n+1; i++)
        y_mu(i) = i * dy;

    // Pressure gradient definition
    if ( globbc == GBC_PRESSURE )
    {
        pg = globbv;
        u = velocityProfile( u );
    }

    else if ( globbc == GBC_BULK_VEL )
    {
        // Guess a pressure gradient
        pg = globbv / -400.0;

        double error = 1000;

        double bulk_vel = 0;   // Calculated bulk velocity

        // Loop until the desired bulk velocity is reached.
        while ( error > errork )
        {
            u = velocityProfile( u );

            bulk_vel = sum( u ) / n;

            error = abs( (bulk_vel - globbv) / globbv );

            // Guess a new pressure gradient, and scale the elements of u
            pg = pg * globbv / bulk_vel;
            u = u * globbv / bulk_vel;

            // Set the ghost points to the appropriate values
            setGhost( &u );
        }
    }
    return u;
}

double CPModel::prandtlLength( double y )
{
    double l_m = lambda * delta;

    if ( y < lambda*delta/kappa )
       l_m = kappa*y;
    else if ( y > diameter - (lambda*delta/kappa) )
       l_m = kappa*(diameter - y);

    return l_m;
}
