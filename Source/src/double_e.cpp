#include <double_e.h>
#include <units.h>

//-----------------------------------------------------------------------------
DoubleETrajectory::DoubleETrajectory(int32_t spd, double freq, int32_t ampl) :
    MatrixTrajectory( (TO_PULSES*spd) / (TO_RPM*freq) ) {
  double t = 1. / freq,                         // s
         sqrt2 = sqrt(2.),
         hsqr2 = sqrt2 / 2.,
         l = xsteplen_/2.,                      // pulses
         k = ampl / (2.*l*sqrt2),               // nondimensional
         d = sqrt(l*l*(k*k+1.)) / 2.;

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

  torch_speed_ = (8. * d + 2. * l) / t; // pulses / s
  torch_speed_ *= TO_RPM / TO_PULSES;
  printf("T: %f l: %f d: %f k: %f Ts: %f\n", t, l, d, k, torch_speed_ );
}

//-----------------------------------------------------------------------------
