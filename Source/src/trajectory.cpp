#include <trajectory.h>

//-----------------------------------------------------------------------------
AbstractTrajectory::AbstractTrajectory(const Vector3D &rotate_vec, double rad_xangle ) : index_(~0), rotation_vec_(rotate_vec) {
  Vector2D xyproj( rotate_vec.x(), rotate_vec.y() );
  Vector3D yline( -rotate_vec.y()/xyproj.length(), 0, 0);
  yline.y() = sqrt(1-yline.x()*yline.x());

  long double alpha = atan2(-yline.x(),yline.y()),
              beta  = atan2(rotate_vec.z(),xyproj.length());

  MatrixLD overx(3), overy(3), overz(3);
  overz(0,0) = overz(1,1) = cos( alpha );
  overz(1,0) = sin(alpha);
  overz(0,1) = -overz(1,0);

  overy(0,0) = overy(2,2) = cos( beta );
  overy(2,0) = sin( beta );
  overy(0,2) = -overy(2,0);

  overx(1,1) = overx(2,2) = cos( rad_xangle );
  overx(1,2) = sin( rad_xangle );
  overx(2,1) = -overx(1,2);

  rotation_matrix_ = overz * overy * overx;
  unrotation_matrix_ = !rotation_matrix_;
}

//-----------------------------------------------------------------------------
bool AbstractTrajectory::getPoint( Vector3D &pos, double &spd, double &progress ) {
  boost::lock_guard<boost::mutex> lock(data_mutex_);
  if( index_ == ~0 ) index_ = 0;
  if( index_ >= positions_.size() ) return false;
  pos = positions_[index_];
  spd = speeds_[index_];
  progress = double(++index_) / positions_.size();
  return true;
}

//-----------------------------------------------------------------------------
void AbstractTrajectory::rotate() {
  PositionVector::iterator it  = positions_.begin(),
                           end = positions_.end();
  for(; it != end; ++it) {
    *it = rotate( *it );
    if( rotation_vec_.x() < 0 ) it->x() *= -1;
  }
}

//-----------------------------------------------------------------------------
Vector3D AbstractTrajectory::rotate( const Vector3D &vec ) const {
  MatrixLD rotated = rotation_matrix_ * MatrixLD(vec);
  return Vector3D( rotated(0,0), rotated(1,0), rotated(2,0) );
}

//-----------------------------------------------------------------------------
Vector3D AbstractTrajectory::unrotate( const Vector3D &vec ) const {
  MatrixLD rotated = unrotation_matrix_ * MatrixLD(vec);
  return Vector3D( rotated(0,0), rotated(1,0), rotated(2,0) );
}

//-----------------------------------------------------------------------------
void AbstractTrajectory::addR( const Vector3D &delta, double spdmm ) { // Relative
  accumulator_ += delta;
  positions_.push_back( accumulator_ );
  speeds_.push_back( spdmm * TO_RPM );
}

//-----------------------------------------------------------------------------
void AbstractTrajectory::addA( const Vector3D &pos, double spdmm ) { // Absolute
  accumulator_ = pos;
  positions_.push_back( accumulator_ );
  speeds_.push_back( spdmm * TO_RPM );
}

//-----------------------------------------------------------------------------
