#ifndef _TRAJECTORY_H_
#define _TRAJECTORY_H_

#include <protocol.h>
#include <math.h>
#include <boost/chrono.hpp>
//#include <master_communicator.h>

//-----------------------------------------------------------------------------
enum TrajectoryControl {
  POSITION = 1,
  VELOCITY = 2,
  SPDPOS   = 3,
  TORQUE   = 4
};

//-----------------------------------------------------------------------------
class AbstractTrajectory {
public:
  AbstractTrajectory() : mode_( SPDPOS ) {}
  TrajectoryControl controlMode() { return mode_; }
  virtual bool finished() = 0;
  virtual AbstractProtocol::ConcurrentCmmd32 speed() = 0;
  virtual AbstractProtocol::ConcurrentCmmd32 position() = 0;
  virtual boost::chrono::milliseconds interval() = 0;
protected:
  double adjustedSpeed( double v, double a, double t) { // all dimensions must be at the same units
    return (a * t - sqrt( a*a*t*t - 4 * a * v * t ) ) / 2.;
  }
private:
  TrajectoryControl mode_;
};

typedef boost::shared_ptr< AbstractTrajectory > AbsTrajectoryPtr;

//-----------------------------------------------------------------------------

#endif
