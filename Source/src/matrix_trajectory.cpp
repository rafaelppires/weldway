#include <matrix_trajectory.h>
#include <units.h>

//-----------------------------------------------------------------------------
MatrixTrajectory::MatrixTrajectory( uint32_t xsteplen ) : step_(0),
    xposbase_(0), xsteplen_(xsteplen), last_(0,0), current_(0,0), yalternate_(false),
    ysignal_(1), moveto_done_(false) {

}

//-----------------------------------------------------------------------------
bool MatrixTrajectory::finished() {
  return current_.x() > 30000;
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
    Coordinate speed = (current_ - last_) * (torch_speed_/d);
    ret[ X_AXIS ] = fabs(speed.x());
    if( speed.y() > 1e-5 ) ret[ Y_AXIS ] = fabs(speed.y());
  } else
    ret[ X_AXIS ] = ret[ Y_AXIS ] = 650; // max speed
  return ret;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 MatrixTrajectory::position() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  int32_t yoffset = 400; // 10 mm
  if( moveto_done_ ) {
    ret[ X_AXIS ] = current_.x();
    if( current_.y() != last_.y() ) ret[ Y_AXIS ] = current_.y() - yoffset;
  } else {
    ret[ X_AXIS ] =  3000;
    ret[ Y_AXIS ] = -yoffset -amplitude_ / 2;
    xposbase_ = ret[X_AXIS];
  }
  return ret;
}

//-----------------------------------------------------------------------------
boost::chrono::milliseconds MatrixTrajectory::interval() {
  uint32_t ret = 0;
  if( !step_ && !moveto_done_ ) {
    moveto_done_ = true;
    ret = 3000;
  } else {
    ++step_;
    ret = 1000. * (last_.distance( current_ )/TO_PULSES) / (torch_speed_/TO_RPM);
  }
  last_ = current_;
  return boost::chrono::milliseconds(ret);
}

//-----------------------------------------------------------------------------
void MatrixTrajectory::add( const Coordinate &c ) {
  trajectory_.push_back(c);
}

//-----------------------------------------------------------------------------
