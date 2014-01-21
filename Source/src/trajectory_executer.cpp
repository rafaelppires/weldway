#include <trajectory_executer.h>
#include <boost/chrono.hpp>
#include <Matrix.h>
using boost::chrono::high_resolution_clock;
using boost::chrono::nanoseconds;
using boost::chrono::milliseconds;

//-----------------------------------------------------------------------------
void TrajectoryExecuter::operator()() {
  positions_.clear();
  speeds_.clear();
  positions_ = trajectory_->positions();
  speeds_ = trajectory_->speeds();

  trajectoryRotate();

  waitFor( gotoInitial() + 200 );
  current_pos_ = trajectory_init_;

  PositionVector::iterator it = positions_.begin(),
                           end = positions_.end();
  Vector3I last_pos(0,0,0), delta(0,0,0);
  Vector3US last_spd(0,0,0);
  uint16_t interval;
  AbstractProtocol::ConcurrentCmmd32 poscmmds, spdcmmds;
  high_resolution_clock::time_point now, start;
  for(; it != end; ++it) {
    start = high_resolution_clock::now();
    delta = *it - last_pos;

    Vector3US spds = getSpeedsAndInterval( delta, interval );
    uint16_t sx = spds.x(), sy = spds.y(), sz = spds.z();

    if( sx && sx != last_spd.x() ) spdcmmds[ X_AXIS ] = sx;
    if( sy && sy != last_spd.y() ) spdcmmds[ Y_AXIS ] = sy;
    if( sz && sz != last_spd.z() ) spdcmmds[ Z_AXIS ] = sz;

    current_pos_ += delta;
    std::cout << *it << " --- " << delta << " curpos: " << current_pos_ << "\n";
    if( delta.x() ) poscmmds[ X_AXIS ] =  current_pos_.x();
    if( delta.y() ) poscmmds[ Y_AXIS ] = -current_pos_.y();
    if( delta.z() ) poscmmds[ Z_AXIS ] =  current_pos_.z();

    comm_->sendSpdCmmds(spdcmmds);
    comm_->sendPosCmmds(poscmmds);
    now = high_resolution_clock::now();

    waitFor( interval - boost::chrono::duration_cast<milliseconds>(now - start).count() );
    last_pos = *it;
    last_spd = spds;
  }
  cancel();
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::waitFor( uint32_t ms ) {

}

//-----------------------------------------------------------------------------
uint32_t TrajectoryExecuter::gotoInitial() {
  AbstractProtocol::ConcurrentCmmd32 poscmmds;
  poscmmds[ X_AXIS ] = trajectory_init_.x();
  poscmmds[ Y_AXIS ] = trajectory_init_.y();
  poscmmds[ Z_AXIS ] = trajectory_init_.z();
  comm_->sendPosCmmds(poscmmds);
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::trajectoryRotate() {
  Vector3I rotate_vector = trajectory_final_ - trajectory_init_;
  long double len = rotate_vector.length(),
              overz_sin = rotate_vector.y() / len,
              overz_cos = rotate_vector.x() / len,
              overy_sin = rotate_vector.z() / len,
              overy_cos = overz_cos;
  std::cout << "ROTATE VECTOR " << rotate_vector << "\n";
  MatrixLD overz(3), overy(3);
  overz(0,0) = overz(1,1) = overz_cos;
  overz(1,0) = overz_sin;
  overz(0,1) = -overz_sin;

  overy(0,0) = overy(2,2) = overy_cos;
  overy(2,0) = overy_sin;
  overy(0,2) = -overy_sin;

  MatrixLD rotateyz( overz * overy );
  PositionVector::iterator it = positions_.begin(),
                           end = positions_.end();
  for(; it != end; ++it) {
    MatrixLD rotated( rotateyz * MatrixLD(*it) );
    *it = Vector3I( 0.5+rotated(0,0), 0.5+rotated(1,0), 0.5+rotated(2,0) );
  }
}

//-----------------------------------------------------------------------------
Vector3US TrajectoryExecuter::getSpeedsAndInterval( Vector3I, uint16_t &interval ) {
  // Find out resultant
  // Decompose to find the axis projection
  // return adjustedSpeed();
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::setLimits(const Vector3I &init, const Vector3I &final ) {
  trajectory_init_ = init;
  trajectory_final_ = final;
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::setCurrent(const Vector3I &last ) {
  current_pos_ = last;
}

//-----------------------------------------------------------------------------
bool TrajectoryExecuter::finished() {
   bool ret;
   {
     boost::lock_guard<boost::mutex> lock(finish_mutex_);
     ret = finished_;
   }
   return ret;
 }

//-----------------------------------------------------------------------------
void TrajectoryExecuter::cancel() {
  boost::lock_guard<boost::mutex> lock(finish_mutex_);
  comm_->stopTorch();
  finished_ = true;
}

//-----------------------------------------------------------------------------
