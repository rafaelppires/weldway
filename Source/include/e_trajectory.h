#ifndef _E_TRAJECTORY_H_
#define _E_TRAJECTORY_H_

#include <trajectory.h>
#include <stdint.h>

class ETrajectory : public AbstractTrajectory {
public:
  ETrajectory(int32_t spd, double freq, int32_t ampl, double total_length) {
    double l = (TO_PULSES*spd) / (TO_RPM*freq);
    int period_count = 0.5 + total_length/l;
    double t = 1. / freq,                         // s
           sqrt2 = sqrt(2.),
           hsqr2 = sqrt2 / 2.,
           k = ampl / (l*sqrt2),                // nondimensional
           d = sqrt(l*l*(k*k+1.)) / 2.;

    sqrt2 *= l;
    hsqr2 *= l;

    double vr = (4. * d + l) / (TO_PULSES*t); // mm / s

    Vector3D acc;

    offset_.x() = 0.5*l;
    offset_.y() = -k*hsqr2;

    add( Vector3D(  0.5*l,  k*hsqr2, 0 ), vr );
    add( Vector3D( -0.5*l,  k*hsqr2, 0 ), vr );
    add( Vector3D( -0.5*l, -k*hsqr2, 0 ), vr );
    for( uint32_t i = 0; i < period_count - 1; ++i ) {
      add( Vector3D(  0.5*l, -k*hsqr2, 0 ), vr );
      add( Vector3D(      l,        0, 0 ), vr );
      add( Vector3D(  0.5*l,  k*hsqr2, 0 ), vr );
      add( Vector3D( -0.5*l,  k*hsqr2, 0 ), vr );
      add( Vector3D( -0.5*l, -k*hsqr2, 0 ), vr );
    }
    add( Vector3D(  0.5*l, -k*hsqr2, 0 ), vr );
    printf("T: %f l: %f d: %f k: %f Ts: %f\n", t, l, d, k, vr );
  }

  Vector3I initialOffset() const {
    return offset_;
  }

private:
  Vector3I offset_;
};

#endif
