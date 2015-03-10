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
  SwitchBackTrajectory(int32_t fwlen, int32_t weldspd, double ratio, uint8_t polarity, TrajectoryTransformPtr tt  ) : AbstractTrajectory(tt) {
    double total_length = tt->length();
    int period_count = 0.5 + 2. * total_length/fwlen;
    double spdmm = weldspd / TO_RPM;
    Vector3D forward(fwlen/2,0,0), backward(-fwlen/2.,0,0);
    double fwspd = (1+2*ratio)*spdmm/ratio,
           bwspd = (1+2*ratio)*spdmm;
    for( int i = 0; i < period_count; ++i ) {
      addR( forward,  fwspd, SincPair(0,(polarity & 0x4) >> 2 ) );
      addR( forward,  fwspd, SincPair(0, polarity & 0x1) );
      addR( backward, bwspd, SincPair(0,(polarity & 0x2) >> 1) );
    }
    rotate();
  }

  void applyCorrection( int32_t fwlen, int32_t weldspd ) {

  }
};

//-----------------------------------------------------------------------------
#endif
