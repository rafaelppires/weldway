#ifndef _TRAJECTORY_H_
#define _TRAJECTORY_H_

#include <protocol.h>
#include <math.h>
#include <boost/chrono.hpp>
#include <vectorxd.h>

//-----------------------------------------------------------------------------
typedef Vector3<uint16_t> Position;
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
  virtual bool torchOn() = 0;

  void setLimits( const Vector3US &init, const Vector3US &final ) {
    trajectory_init_ = init;
    trajectory_final_ = final;
  }

  void setCurrent( int32_t pos ) {
    current_pos_ = pos;
  }

protected:
  double adjustedSpeed( double v, double a, double t) { // all dimensions must be at the same units
    return (a * t - sqrt( a*a*t*t - 4 * a * v * t ) ) / 2.;
  }
  Vector3US trajectory_init_, trajectory_final_;
  int32_t current_pos_;

private:
  TrajectoryControl mode_;
};

typedef boost::shared_ptr< AbstractTrajectory > AbsTrajectoryPtr;

//-----------------------------------------------------------------------------

#endif
