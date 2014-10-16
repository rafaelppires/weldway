#include <trajectory.h>

//-----------------------------------------------------------------------------
AbstractTrajectory::AbstractTrajectory( TrajectoryTransformPtr tt ) : index_(~0), transform_(tt) {
}

//-----------------------------------------------------------------------------
void AbstractTrajectory::setupMatrixes( double alpha ) {
  Vector2D xzproj( rotation_vec_.x(), rotation_vec_.z() );
  long double beta = atan2( xzproj.y(), xzproj.x() ),
              gama = atan2( rotation_vec_.y(), xzproj.length() );

  MatrixLD overx(3), overy(3), overz(3);
  overz(0,0) = overz(1,1) = cos( gama );
  overz(1,0) = sin(gama);
  overz(0,1) = -overz(1,0);

  overy(0,0) = overy(2,2) = cos( beta );
  overy(2,0) = sin( beta );
  overy(0,2) = -overy(2,0);

  overx(1,1) = overx(2,2) = cos( alpha );
  overx(1,2) = sin( alpha );
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

  //Eliminate collinear points when speeds of both segments are equal
  size_t psz = positions_.size();
  if( psz > 2 && speeds_.back() == speeds_[ speeds_.size()-2 ] ) {
    Vector3D A  = positions_.back(),
             B  = positions_[ psz-2 ],
             C  = positions_[ psz-3 ],
             BA = A - B,
             BC = C - B;
    if( BA.cross( BC ).length() < 1e-5 && BA.dot( BC ) < 0 ) {
      positions_.erase( positions_.begin()+psz-2 );
      speeds_.pop_back();
    }
  }
}

//-----------------------------------------------------------------------------
void AbstractTrajectory::addA( const Vector3D &pos, double spdmm ) { // Absolute
  accumulator_ = pos;
  positions_.push_back( accumulator_ );
  speeds_.push_back( spdmm * TO_RPM );
}
//-----------------------------------------------------------------------------
void AbstractTrajectory::setReference( const Vector3D &v ) {
  accumulator_ = v;
}

//-----------------------------------------------------------------------------
void AbstractTrajectory::setReference() {
  accumulator_ = positions_.back();
}

//-----------------------------------------------------------------------------
void AbstractTrajectory::eraseFrom(uint32_t idx ) {
  positions_.erase( positions_.begin() + idx, positions_.end() );
  speeds_.erase( speeds_.begin() + idx, speeds_.end() );
}

//-----------------------------------------------------------------------------
