#ifndef _RHOMBUS_H_
#define _RHOMBUS_H_

#include <trajectory.h>
#include <vectorxd.h>
#include <linear_transform.h>

class Rhombus : public AbstractTrajectory {
public:
  Rhombus(int32_t a, int32_t l, uint8_t n, double wspeed ,   TrajectoryTransformPtr tt ) : AbstractTrajectory(tt) {
    double total_length = tt->length();
    int32_t xstep = l / (4 * n);
    int period_count = 0.5 + 2 * total_length/l;
    double xspeed = 3 * wspeed,
           yspeed = 6 * n * a * wspeed / l,
           spdmm = sqrt( xspeed*xspeed + yspeed*yspeed ) / TO_RPM;

    for( uint16_t k = 0; k < period_count; ++k ) {
      int ysig = (n%2==0 || k%2 == 0) ? 1 : -1;
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

  //-----------------------------------------------------------------------------
  static void draft(PositionVector &out, int32_t a, int32_t l, uint8_t n, double wspeed ) {
    Rhombus e( a,l,n,wspeed, TrajectoryTransformPtr( new LinearTransform(Vector3D(4*l,0,0),0)) );
    out.clear();
    out.push_back( e.initialOffset() );
    out.insert( out.end(), e.positions_.begin(), e.positions_.end() );
  }

  //-----------------------------------------------------------------------------
};

#endif
