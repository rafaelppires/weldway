#include <matrix_trajectory.h>

//-----------------------------------------------------------------------------
MatrixTrajectory::MatrixTrajectory() : step_(0) {

}

//-----------------------------------------------------------------------------
bool MatrixTrajectory::finished() {
  return false;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 MatrixTrajectory::speed() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  return ret;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 MatrixTrajectory::position() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  int idx = _step % trajectory_.size();
  ret[ X_AXIS ] = trajectory_[idx].first;
  ret[ Y_AXIS ] = trajectory_[idx].second;
  return ret;
}

//-----------------------------------------------------------------------------
boost::chrono::milliseconds MatrixTrajectory::interval() {
  ++_step;
  return boost::chrono::milliseconds(0);
}

//-----------------------------------------------------------------------------
void MatrixTrajectory::add( const Coordinate &c ) {
  trajectory_.push_back(c);
}

//-----------------------------------------------------------------------------
