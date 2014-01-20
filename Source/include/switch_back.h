#ifndef _SWITCHBACK_H_
#define _SWITCHBACK_H_

#include <protocol.h>
#include <trajectory.h>
#include <units.h>
#include <boost/chrono.hpp>

class SwitchBackTrajectory : public AbstractTrajectory {
public:
  SwitchBackTrajectory( int32_t, int32_t );

private:

};

#endif
