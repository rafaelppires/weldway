#ifndef _RHOMBUS_H_
#define _RHOMBUS_H_

#include <trajectory.h>
#include <vectorxd.h>

class Rhombus : public AbstractTrajectory {
public:
  Rhombus(int32_t a, int32_t l, uint8_t n, double wspeed , const Vector3D &rotate_vec, double deg_xang ) : AbstractTrajectory(rotate_vec, deg_xang) {
    double total_length = rotate_vec.length();
    int32_t xstep = l / (4 * n);
    int period_count = 0.5 + 2 * total_length/l;
    double xspeed = 3 * wspeed,
           yspeed = 6 * n * a * wspeed / l,
           spdmm = sqrt( xspeed*xspeed + yspeed*yspeed ) / TO_RPM;

    for( uint16_t k = 0; k < period_count; ++k ) {
      int ysig = k%2 == 0 ? 1 : -1;
      for(uint8_t i = 0; i < n; ++i) {
        addR( Vector3D( xstep,  ysig*a/2., 0 ), spdmm );
        addR( Vector3D( 2*xstep,  -ysig*a, 0 ), spdmm );
        addR( Vector3D( xstep,  ysig*a/2., 0 ), spdmm );
      }

      for(uint8_t i = 0; i < n; ++i) {
        int sig = i%2 == 0 ? 1 : -1;
        addR( Vector3D( -xstep,  ysig*sig*a/2., 0 ), spdmm );
        addR( Vector3D( -xstep, -ysig*sig*a/2., 0 ), spdmm );
      }
    }
    rotate();
  }

  void applyCorrection( int32_t a, int32_t l, uint8_t n, double wspeed ) {

  }
};

#endif