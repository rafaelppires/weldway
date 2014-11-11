#ifndef _TRAJECTORY_EXECUTER_H_
#define _TRAJECTORY_EXECUTER_H_

#include <protocol.h>
#include <trajectory.h>
#include <units.h>
#include <boost/thread.hpp>
#include <boost/function.hpp>

using boost::chrono::high_resolution_clock;
using boost::chrono::milliseconds;
//-----------------------------------------------------------------------------
class SingleShotSinc {
public:
  typedef boost::function1<void,const boost::system::error_code&> CallbackFunc;
  SingleShotSinc(boost::shared_ptr< AbstractProtocol > comm, long t, uint8_t v, high_resolution_clock::time_point b) : comm_(comm), t_(t), v_(v), beg_(b) {}

  void operator()() {
    boost::this_thread::sleep_for(milliseconds(t_));
    comm_->setSinc( v_ );
    std::cout << "bye! " << boost::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - beg_).count() << "ms ";
  }
private:
  long t_;
  uint8_t v_;
  boost::shared_ptr< AbstractProtocol > comm_;
  high_resolution_clock::time_point beg_;
};

//-----------------------------------------------------------------------------
class TrajectoryExecuter {
public:
  TrajectoryExecuter( AbsTrajectoryPtr t, boost::shared_ptr< AbstractProtocol > comm );
  void operator()();
  bool finished();
  void cancel();
  void setLimits(const Vector3I &init);
  void setCurrent(const Vector3I &last , const Vector2I &last_torch);
  void setAngularOffset( double );
  Vector3US getSpeedsAndInterval(const Vector3D &delta, uint16_t &interval, double res_spd, const Vector2I &delta_torch, Vector2US &spd_torch);
  void setProgressCallback( boost::function<void(double)> cb ) { progress_callback_ = cb; }
  void addLinearOffset( const Vector3D &offset );
  void scheduleSinc( double t, uint8_t v );

private:
  void waitFor( uint32_t ms );
  Vector3D gotoInitial();
  void deliverSpeedsAndPositions(const Vector3I &delta, const Vector3US &speeds , const Vector2I &delta_torch, const Vector2US &spd_torch);

  // all dimensions must be at the same base units
  uint16_t fixSpeed( double v, double a, double t );
  static double adjustedSpeed( double v, double a, double t) {
    return (a * t - sqrt( a*a*t*t - 4 * a * v * t ) ) / 2.;
  }

  AbsTrajectoryPtr trajectory_;
  boost::shared_ptr< AbstractProtocol > comm_;
  boost::mutex finish_mutex_, correction_mutex_;
  bool finished_, offset_updated_;
  Vector3I current_pos_, trajectory_init_, trajectory_final_;
  Vector2I current_torch_;
  Vector3D acceleration_, offset_, accumulated_offset_;
  Vector3US last_spd_;
  Vector2US last_torch_spd_;
  double overx_angle_;
  boost::function<void(double)> progress_callback_;
  bool controls_torch_;
};

#endif
