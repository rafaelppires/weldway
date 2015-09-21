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
  SwitchBackTrajectory(int32_t fwlen, int32_t bwlen, int32_t weldspd, double ratio, uint8_t polarity, TrajectoryTransformPtr tt  ) : AbstractTrajectory(tt) {
    double total_length = tt->length();
    if( bwlen >= fwlen ) bwlen = fwlen - 1;
    int period_count = 0.5 + total_length/(fwlen-bwlen);
    double spdmm = weldspd / TO_RPM;
    Vector3D forward(fwlen/2,0,0), backward(-bwlen,0,0);
    double mult  = (bwlen+2.*fwlen)/double(fwlen-bwlen),
           fwspd = mult*spdmm/ratio,
           bwspd = mult*spdmm;
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
