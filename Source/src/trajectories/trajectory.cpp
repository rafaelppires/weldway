#include <trajectory.h>

//-----------------------------------------------------------------------------
AbstractTrajectory::AbstractTrajectory( TrajectoryTransformPtr tt ) : index_(~0), transform_(tt), controls_torch_(tt->controlsTorch()) {
}

//-----------------------------------------------------------------------------
bool AbstractTrajectory::getPoint(Vector3D &pos, double &spd, Vector2D &torch, SincPair &s, double &progress ) {
  boost::lock_guard<boost::mutex> lock(data_mutex_);
  if( index_ == ~0 ) index_ = 0;
  if( index_ >= positions_.size() ) return false;
  pos   = positions_[index_];
  spd   = speeds_[index_];
  if( controls_torch_ ) torch = torch_pos_[index_];
  s = sinc_[index_];
  progress = double(++index_) / positions_.size();
  return true;
}

//-----------------------------------------------------------------------------
void AbstractTrajectory::rotate() {
  PositionVector::iterator it  = positions_.begin(),
                           end = positions_.end();
  for(; it != end; ++it) {
    *it = transform_->transform( *it );
    if( controls_torch_ )
      torch_pos_.push_back( transform_->gettorch() );
  }
}

//-----------------------------------------------------------------------------
void AbstractTrajectory::addR(const Vector3D &delta, double spdmm , const SincPair &s) { // Relative
  accumulator_ += delta;
  positions_.push_back( accumulator_ );
  speeds_.push_back( spdmm * TO_RPM );
  sinc_.push_back( s );

  /* Eliminate collinear points when speeds of both segments are equal and polarity
   * signals are innocuous.
   * Points must be collinear (null cross product) and there must be no direction
   * inversion (negative dot product). "Switch back" trajectory is therefore safe.
   **/
  size_t psz = positions_.size();
  if( psz > 2 && // there are at least 3 points
      speeds_.back() == speeds_[ speeds_.size()-2 ] && // the two last speeds are the same
      (s.first < 0 || s.first > 1. || (sinc_[ sinc_.size()-2 ].first >= 0 && sinc_[ sinc_.size()-2 ].first <= 1.0 &&
                                       s.second == sinc_[ sinc_.size()-2 ].second ) ) ) { // there is no polarity change
    Vector3D A  = positions_.back(),
             B  = positions_[ psz-2 ],
             C  = positions_[ psz-3 ],
             BA = A - B,
             BC = C - B;
    if( BA.cross( BC ).length() < 1e-5 && BA.dot( BC ) < 0 ) {
      positions_.erase( positions_.begin()+psz-2 );
      speeds_.pop_back();
      sinc_.pop_back();
    }
  }
}

//-----------------------------------------------------------------------------
void AbstractTrajectory::addA( const Vector3D &pos, double spdmm, const SincPair &s ) { // Absolute
  accumulator_ = pos;
  positions_.push_back( accumulator_ );
  speeds_.push_back( spdmm * TO_RPM );
  sinc_.push_back( s );
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
