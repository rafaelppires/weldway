#ifndef _DOUBLE_E_H_
#define _DOUBLE_E_H_

#include <trajectory.h>
#include <stdint.h>

class DoubleETrajectory : public AbstractTrajectory {
public:
  DoubleETrajectory(int32_t spd, double freq, int32_t ampl, const Vector3D &rotate_vec, double deg_xang ) : AbstractTrajectory(rotate_vec, deg_xang) {
    double total_length = rotate_vec.length();
    double lambda = (TO_PULSES*spd) / (TO_RPM*freq);
    int period_count = 0.5 + total_length/lambda;
    double t = 1. / freq,                      // s
           hsqr2 = sqrt(2.) / 2.,
           l = lambda/2.,                      // pulses
           k = ampl / (4.*l*hsqr2),            // nondimensional
           d = sqrt(l*l*(k*k+1.)) / 2.;

    hsqr2 *= l;
    double vr = (8. * d + 2. * l) / (TO_PULSES * t); // mm / s

    for( uint32_t i = 0; i < period_count; ++i ) {
      addR( Vector3D(      l,        0, 0 ), vr );
      addR( Vector3D(  hsqr2,  k*hsqr2, 0 ), vr );
      addR( Vector3D( -hsqr2,  k*hsqr2, 0 ), vr );
      addR( Vector3D( -hsqr2, -k*hsqr2, 0 ), vr );
      addR( Vector3D(  hsqr2, -k*hsqr2, 0 ), vr );

      addR( Vector3D(      l,        0, 0 ), vr );
      addR( Vector3D(  hsqr2, -k*hsqr2, 0 ), vr );
      addR( Vector3D( -hsqr2, -k*hsqr2, 0 ), vr );
      addR( Vector3D( -hsqr2,  k*hsqr2, 0 ), vr );
      addR( Vector3D(  hsqr2,  k*hsqr2, 0 ), vr );
    }

    rotate();
  }

  void applyCorrection( int32_t spd, double freq, int32_t ampl ) {

  }

};

#endif
