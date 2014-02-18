#ifndef _E_TRAJECTORY_H_
#define _E_TRAJECTORY_H_

#include <trajectory.h>
#include <stdint.h>

class ETrajectory : public AbstractTrajectory {
public:
  ETrajectory(int32_t spd, double freq, int32_t ampl,  const Vector3D &rotate_vec, double deg_xang ) : AbstractTrajectory(rotate_vec, deg_xang)  {
    double total_length = rotate_vec.length();
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

    addR( Vector3D(  0.5*l,  k*hsqr2, 0 ), vr );
    addR( Vector3D( -0.5*l,  k*hsqr2, 0 ), vr );
    addR( Vector3D( -0.5*l, -k*hsqr2, 0 ), vr );
    for( uint32_t i = 0; i < period_count - 1; ++i ) {
      addR( Vector3D(  0.5*l, -k*hsqr2, 0 ), vr );
      addR( Vector3D(      l,        0, 0 ), vr );
      addR( Vector3D(  0.5*l,  k*hsqr2, 0 ), vr );
      addR( Vector3D( -0.5*l,  k*hsqr2, 0 ), vr );
      addR( Vector3D( -0.5*l, -k*hsqr2, 0 ), vr );
    }
    addR( Vector3D(  0.5*l, -k*hsqr2, 0 ), vr );
    printf("T: %f l: %f d: %f k: %f Ts: %f\n", t, l, d, k, vr );

    rotate();
  }

  Vector3I initialOffset() const {
    int sig = rotation_vec_.x() / fabs(rotation_vec_.x());
    return rotate( Vector3D(offset_.x()*sig, offset_.y(), offset_.z()) );
  }

  void applyCorrection( int32_t spd, double freq, int32_t ampl ) {

  }

private:
  Vector3D offset_;
};

#endif
