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
  size_t spd_idx = 0,
         spds_sz = speeds_.size();

  trajectoryRotate();
  waitFor( gotoInitial() + 200 );

  PositionVector::iterator it = positions_.begin(),
                           end = positions_.end();
  Vector3D last_pos(0,0,0), delta(0,0,0);
  uint16_t interval;

  high_resolution_clock::time_point now, start;
  for(; it != end; ++it) {
    start = high_resolution_clock::now();
    delta = *it - last_pos;

    Vector3US spds = getSpeedsAndInterval( delta, interval, speeds_[spd_idx++] );
    if( spd_idx >= spds_sz ) spd_idx = spds_sz - 1;

    deliverSpeedsAndPositions( delta, spds );
    std::cout << *it << " --- " << delta << " curpos: " << current_pos_ << " spd: " << spds << " inter: " << interval << "\n";
    fflush(stdout);
    now = high_resolution_clock::now();

    waitFor( interval - boost::chrono::duration_cast<milliseconds>(now - start).count() );
    last_pos = *it;
  }
  cancel();
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::deliverSpeedsAndPositions( const Vector3I &delta, const Vector3US &speeds ) {
  AbstractProtocol::ConcurrentCmmd32 poscmmds, spdcmmds;
  uint16_t sx = speeds.x(), sy = speeds.y(), sz = speeds.z();
  int32_t  dx = delta.x(), dy = delta.y(), dz = delta.z();

  if( dx && sx && sx != last_spd_.x() ) spdcmmds[ X_AXIS ] = sx;
  if( dy && sy && sy != last_spd_.y() ) spdcmmds[ Y_AXIS ] = sy;
  if( dz && sz && sz != last_spd_.z() ) spdcmmds[ Z_AXIS ] = sz;

  current_pos_ += delta;
  if( delta.x() ) poscmmds[ X_AXIS ] =  current_pos_.x();
  if( delta.y() ) poscmmds[ Y_AXIS ] = -current_pos_.y();
  if( delta.z() ) poscmmds[ Z_AXIS ] =  current_pos_.z();

  comm_->sendSpdCmmds(spdcmmds);
  comm_->sendPosCmmds(poscmmds);

  last_spd_ = Vector3US(spdcmmds[ X_AXIS ], spdcmmds[ Y_AXIS ], spdcmmds[ Z_AXIS ]);
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::waitFor( uint32_t ms ) {
  std::cout << "Inter: " << ms << "\n";
  boost::this_thread::sleep_for( boost::chrono::milliseconds( ms ) );
}

//-----------------------------------------------------------------------------
uint16_t TrajectoryExecuter::gotoInitial() {
  uint16_t ret;
  Vector3I delta = trajectory_init_ - current_pos_;
  deliverSpeedsAndPositions( delta, getSpeedsAndInterval( delta, ret, 650 ) );
  return ret;
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::trajectoryRotate() {
  Vector3I rotate_vector = trajectory_final_ - trajectory_init_;
  long double len = rotate_vector.length(),
              overz_sin = rotate_vector.y() / len,
              overz_cos = rotate_vector.x() / len,
              overy_sin = rotate_vector.z() / len,
              overy_cos = overz_cos,
              overx_sin = sin( 30 * 3.14 /180 ),
              overx_cos = cos( 30 * 3.14 /180 );

  MatrixLD overx(3), overy(3), overz(3);
  overz(0,0) = overz(1,1) = overz_cos;
  overz(1,0) =  overz_sin;
  overz(0,1) = -overz_sin;

  overy(0,0) = overy(2,2) = overy_cos;
  overy(2,0) =  overy_sin;
  overy(0,2) = -overy_sin;

  overx(1,1) = overx(2,2) = overx_cos;
  overx(1,2) =  overx_sin;
  overx(2,1) = -overx_sin;

  MatrixLD rotateyz( overx * overy * overz );
  PositionVector::iterator it  = positions_.begin(),
                           end = positions_.end();
  for(; it != end; ++it) {
    MatrixLD rotated( rotateyz * MatrixLD(*it) );
    *it = Vector3I( 0.5+rotated(0,0), 0.5+rotated(1,0), 0.5+rotated(2,0) );
    if( trajectory_final_.x() < trajectory_init_.x() ) it->x() *= -1;
  }
}

//-----------------------------------------------------------------------------
Vector3US TrajectoryExecuter::getSpeedsAndInterval( const Vector3D &delta, uint16_t &interval, double res_spd ) {
  // Decompose to find the axis projection
  Vector3D vr( Vector3D(delta).unary() );
  vr *= res_spd / TO_RPM; // mm/s
  // Adjust Speed
  interval = 0.5 + 1000. * delta.length() * TO_RPM / (TO_PULSES * res_spd); // ms
  Vector3US ret;
  //                   mm/s         mm/s²              s
  ret.x() = fixSpeed( vr.x(), acceleration_.x(), interval/1000. );
  ret.y() = fixSpeed( vr.y(), acceleration_.y(), interval/1000. );
  ret.z() = fixSpeed( vr.z(), acceleration_.z(), interval/1000. );
  return ret;
}
//-----------------------------------------------------------------------------
uint16_t TrajectoryExecuter::fixSpeed( double v, double a, double t ) {
  if( a * t / v > 4. ) return 0.5 + adjustedSpeed(v,a,t) * TO_RPM;
  return 0;
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
