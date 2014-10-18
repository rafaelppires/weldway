#ifndef _SWITCHBACK_H_
#define _SWITCHBACK_H_

#include <protocol.h>
#include <trajectory.h>
#include <units.h>
#include <boost/chrono.hpp>

//-----------------------------------------------------------------------------
class LinearTrajectory : public AbstractTrajectory {
public:
  LinearTrajectory( int32_t weldspd,  TrajectoryTransformPtr tt  ) : AbstractTrajectory(tt) {
    addR( Vector3D( tt->length(), 0, 0 ), weldspd / TO_RPM );
    rotate();
  }

  void applyCorrection( int32_t weldspd ) {

  }
};

//-----------------------------------------------------------------------------
class SwitchBackTrajectory : public AbstractTrajectory {
public:
  SwitchBackTrajectory(int32_t fwlen, int32_t weldspd, TrajectoryTransformPtr tt  ) : AbstractTrajectory(tt) {
    double total_length = tt->length();
    int period_count = 0.5 + 2. * total_length/fwlen;
    double spdmm = weldspd / TO_RPM;
    Vector3D forward(fwlen,0,0), backward(-fwlen/2.,0,0);
    for( int i = 0; i < period_count; ++i ) {
      addR( forward, 4*spdmm );
      addR( backward, 2*spdmm );
    }
    rotate();
  }

  void applyCorrection( int32_t fwlen, int32_t weldspd ) {

  }
};

//-----------------------------------------------------------------------------
#endif
