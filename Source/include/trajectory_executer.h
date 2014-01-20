#ifndef _TRAJECTORY_EXECUTER_H_
#define _TRAJECTORY_EXECUTER_H_

#include <protocol.h>
#include <trajectory.h>
#include <units.h>
#include <boost/thread.hpp>

class TrajectoryExecuter {
public:
  TrajectoryExecuter( AbsTrajectoryPtr t, boost::shared_ptr< AbstractProtocol > comm )
      : trajectory_(t), comm_(comm), finished_(false) {
    acceleration_.x() = acceleration_.y() = acceleration_.z() = 4000 / (0.025 * TO_RPM);
  }

  void operator()();
  bool finished();
  void cancel();
  void setLimits(const Vector3I &init, const Vector3I &final );
  void setCurrent( const Vector3I &last );
  Vector3US getSpeedsAndInterval(Vector3I , uint16_t &interval);

private:
  void waitFor( uint32_t ms );
  uint32_t gotoInitial();
  void trajectoryRotate();

  // all dimensions must be at the same base units
  double adjustedSpeed( double v, double a, double t) {
    return (a * t - sqrt( a*a*t*t - 4 * a * v * t ) ) / 2.;
  }

  AbsTrajectoryPtr trajectory_;
  boost::shared_ptr< AbstractProtocol > comm_;
  boost::mutex finish_mutex_;
  bool finished_;
  Vector3I current_pos_, trajectory_init_, trajectory_final_;
  PositionVector positions_;
  SpeedVector speeds_;
  Vector3D acceleration_;
};

#endif
