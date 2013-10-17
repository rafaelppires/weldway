#ifndef _DOUBLE_E_H_
#define _DOUBLE_E_H_

#include <trajectory.h>
#include <stdint.h>
#include <matrix_trajectory.h>

class DoubleETrajectory : public MatrixTrajectory {
public:
  DoubleETrajectory(int32_t spd, double freq, int32_t ampl) {
    double t = 1. / freq,                         // s
           sqrt2 = sqrt(2.),
           hsqr2 = sqrt2 / 2.,
           l = t * (TO_PULSES * spd/TO_RPM) / 2., // pulses
           k = ampl / (2.*l*sqrt2),               // nondimensional
           d = sqrt(l*l*(k*k+2.)) / 2.;

    sqrt2 *= l;
    hsqr2 *= l;

    add( Coordinate( l, 0 ) );
    add( Coordinate( l+hsqr2, k*hsqr2 ) );
    add( Coordinate( l, k*sqrt2 ) );
    add( Coordinate( l-hsqr2, k*hsqr2 ) );
    add( Coordinate( l, 0 ) );
    
    add( Coordinate( 2*l, 0 ) );
    add( Coordinate( 2*l+hsqr2, -k*hsqr2 ) );
    add( Coordinate( 2*l, -k*sqrt2 ) );
    add( Coordinate( 2*l-hsqr2, -k*hsqr2 ) );
    add( Coordinate( 2*l, 0 ) );
    
    double torch_speed = (8. * d + 2. * l) / t; // pulses / s
    y0_interval_ = 1000. * l / torch_speed; // ms - interval when y is zero
    other_interval_ = 1000. * d / torch_speed; // ms - interval when y is not zero
    printf("T: %f t(y0): %f t(*): %f l: %f d: %f k: %f\n", t, y0_interval_, other_interval_, l, d, k );
  }

private:
  double y0_interval_, other_interval_;
};

#endif
