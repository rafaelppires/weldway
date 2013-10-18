#include <matrix_trajectory.h>

//-----------------------------------------------------------------------------
MatrixTrajectory::MatrixTrajectory( uint32_t xsteplen ) : step_(0),
    xposbase_(0), xsteplen_(xsteplen), last_(0,0), current_(0,0) {

}

//-----------------------------------------------------------------------------
bool MatrixTrajectory::finished() {
  return false;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 MatrixTrajectory::speed() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  int idx = step_ % trajectory_.size();
  if( step_ && !idx )
    xposbase_ += xsteplen_;
  current_ = Coordinate( xposbase_ + trajectory_[idx].x(),
                         trajectory_[idx].y() );
  double d = last_.distance( current_ );
  Coordinate speed = (current_ - last_) * (torch_speed_/d);
  ret[ X_AXIS ] = speed.x();
  ret[ Y_AXIS ] = speed.y();
  return ret;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 MatrixTrajectory::position() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  ret[ X_AXIS ] = current_.x();
  ret[ Y_AXIS ] = current_.y();
  return ret;
}

//-----------------------------------------------------------------------------
boost::chrono::milliseconds MatrixTrajectory::interval() {
  ++step_;
  return boost::chrono::milliseconds(0);
}

//-----------------------------------------------------------------------------
void MatrixTrajectory::add( const Coordinate &c ) {
  trajectory_.push_back(c);
}

//-----------------------------------------------------------------------------
