#include <e_trajectory.h>

//-----------------------------------------------------------------------------
ETrajectory::ETrajectory( double spd, double l, double ampl,
                          const Vector3D &rotate_vec, double deg_xang ) :
                                    AbstractTrajectory(rotate_vec, deg_xang)  {
  double total_length = rotate_vec.length();
  int period_count = 0.5 + total_length/l;
  double yoff = ampl / 3.,
         vr = spd*(2*Vector2D(l,2*yoff).length()+2*Vector2D(l/2.,yoff).length()) / (TO_RPM*l);

  offset_.x() = 0;
  offset_.y() = -ampl / 2.;

  accumulator_ = offset_;

  addRepeatable( period_count, l, yoff, vr );
  rotate();
}

//-----------------------------------------------------------------------------
Vector3I ETrajectory::initialOffset() const {
  int sig = rotation_vec_.x() / fabs(rotation_vec_.x());
  return rotate( Vector3D(offset_.x()*sig, offset_.y(), offset_.z()) );
}

//-----------------------------------------------------------------------------
void ETrajectory::applyCorrection( double spd, double l, double ampl ) {
  boost::lock_guard<boost::mutex> lock(data_mutex_);
  if( index_ < 1 || index_ > positions_.size() - 2 ) return;
  Vector3D diff, cur, prev;
  uint32_t idx = index_;

  prev = unrotate( positions_[idx-1] );
  do {
    cur = unrotate( positions_[idx++] );
    diff = cur - prev;
    if( fabs(diff.x())<1e-5 ) diff.x() = 0;
    if( fabs(diff.y())<1e-5 ) diff.y() = 0;
    prev = cur;
  } while( idx < positions_.size() && (diff.x() <= 0 || diff.y() >= 0) );

  eraseFrom( idx );

  double yoff = ampl / 3.,
         vr = spd*(2*Vector2D(l,2*yoff).length()+2*Vector2D(l/2.,yoff).length()) / (TO_RPM*l);

  int period_count = 0.5 + (rotation_vec_.length() - cur.x())/l;
  PositionVector::reference last = positions_.back();
  last = cur;
  last.y() = -ampl / 2.;
  setReference();
  addRepeatable( period_count, l, yoff, vr );

  for( int i = idx-1; i < positions_.size(); ++i )
    positions_[i] = rotate(positions_[i]);
}

//-----------------------------------------------------------------------------
void ETrajectory::addRepeatable( uint16_t count, double l, double yoff, double vr ) {
  for( uint32_t i = 0; i < count; ++i ) {
    addR( Vector3D(      l,  2*yoff, 0 ), vr );
    addR( Vector3D(  -l/2.,    yoff, 0 ), vr );
    addR( Vector3D(  -l/2.,   -yoff, 0 ), vr );
    addR( Vector3D(      l, -2*yoff, 0 ), vr );
  }
}

//-----------------------------------------------------------------------------
void ETrajectory::draft( PositionVector &out, double spd, double l, double ampl ) {
  ETrajectory e( spd, l, ampl, Vector3D(4*l,0,0), 0);
  out.clear();
  out.push_back( e.initialOffset() );
  out.insert( out.end(), e.positions_.begin(), e.positions_.end() );
}
//-----------------------------------------------------------------------------
