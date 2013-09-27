#ifndef _TRAJECTORY_H_
#define _TRAJECTORY_H_

#include <protocol.h>
#include <boost/chrono.hpp>
#include <master_communicator.h>

enum TrajectoryControl {
  POSITION = 1,
  VELOCITY = 2,
  SPDPOS   = 3,
  TORQUE   = 4
};

class AbstractTrajectory {
public:
  AbstractTrajectory() : mode_( SPDPOS ) {}
  TrajectoryControl controlMode() { return mode_; }
  virtual bool finished() {}
  virtual uint16_t speed() {}
  virtual AbstractProtocol::ConcurrentCmmd position() {}
  virtual boost::chrono::milliseconds interval() {}
private:
  TrajectoryControl mode_;
};

#endif
