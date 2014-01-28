#ifndef _SWITCHBACK_H_
#define _SWITCHBACK_H_

#include <protocol.h>
#include <trajectory.h>
#include <units.h>
#include <boost/chrono.hpp>

class SwitchBackTrajectory : public AbstractTrajectory {
public:
  SwitchBackTrajectory(int32_t fwlen, int32_t weldspd, double total_length) {
    int period_count = 0.5 + 2. * total_length/fwlen;
    double spdmm = weldspd / TO_RPM;
    Vector3D forward(fwlen,0,0), backward(fwlen/2.,0,0);
    for( int i = 0; i < period_count; ++i ) {
      add( forward, 4*spdmm );
      add( backward, 2*spdmm );
    }
  }
};

#endif
