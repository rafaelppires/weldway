#include <matrix_trajectory.h>
#include <units.h>

//-----------------------------------------------------------------------------
MatrixTrajectory::MatrixTrajectory( uint32_t xsteplen ) : step_(0),
    xposbase_(0), xsteplen_(xsteplen), last_(0,0), current_(0,0), yalternate_(false),
    ysignal_(1), moveto_done_(false) {

}

//-----------------------------------------------------------------------------
bool MatrixTrajectory::finished() {
  return current_.x() > trajectory_final_;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 MatrixTrajectory::speed() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  if( moveto_done_ ) {
    int idx = step_ % trajectory_.size();
    if( step_ && !idx ) {
      xposbase_ += xsteplen_;
      ysignal_ *= -1;
      if( !yalternate_ ) ysignal_ = 1;
    }
    current_ = Coordinate(  xposbase_ + trajectory_[idx].x(),
                           - ysignal_ * trajectory_[idx].y() );
    double d = last_.distance( current_ );
    interval_ = 1000. * (d/TO_PULSES) / (torch_speed_/TO_RPM);
    Coordinate speed = (current_ - last_) * (torch_speed_/d);
    double ax, ay = 4000 / (0.05 * TO_RPM); ax = ay;
    ret[ X_AXIS ] =   adjustedSpeed( fabs(speed.x())/TO_RPM, ax, interval_/1000.) * TO_RPM;
    if( speed.y() > 1e-5 )
      ret[ Y_AXIS ] = adjustedSpeed( fabs(speed.y())/TO_RPM, ay, interval_/1000.) * TO_RPM;
  } else
    ret[ X_AXIS ] = ret[ Y_AXIS ] = 650; // max speed
  return ret;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 MatrixTrajectory::position() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  int32_t yoffset = trajectory_init_.y(); // 10 mm
  if( moveto_done_ ) {
    ret[ X_AXIS ] = current_.x();
    if( current_.y() != last_.y() ) ret[ Y_AXIS ] = current_.y() - yoffset;
  } else {
    ret[ X_AXIS ] =  trajectory_init_.x();
    ret[ Y_AXIS ] = -yoffset -amplitude_ / 2;
    current_.x() = xposbase_ = ret[X_AXIS];
  }
  return ret;
}

//-----------------------------------------------------------------------------
boost::chrono::milliseconds MatrixTrajectory::interval() {
  uint32_t ret = 0;
  if( !step_ && !moveto_done_ ) {
    moveto_done_ = true;
    ret = 500 + 1000. * (fabs(current_pos_-trajectory_init_.x())/TO_PULSES) / (650/TO_RPM);
  } else {
    ++step_;
    ret = interval_;
  }
  last_ = current_;
  return boost::chrono::milliseconds(ret);
}

//-----------------------------------------------------------------------------
void MatrixTrajectory::add( const Coordinate &c ) {
  trajectory_.push_back(c);
}

//-----------------------------------------------------------------------------
