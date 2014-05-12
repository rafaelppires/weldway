#include <bricks.h>
//-----------------------------------------------------------------------------
BricksTrajectory::BricksTrajectory( double spd, double ampl,
                                    const Vector3D &rotate_vec, double deg_xang ) :
    AbstractTrajectory(rotate_vec, deg_xang) {

  double sqsz = ampl / 2, lmbd = 6*sqsz, total = rotate_vec.length();
  offset_ = Vector3D( 0, sqsz, 0 );
  setReference( offset_ );

  addRepeatable( size_t(0.5+total/lmbd), sqsz, 3*spd/TO_RPM );
  rotate();
}

//-----------------------------------------------------------------------------
void BricksTrajectory::addRepeatable( size_t n, double sqr, double spd ) {
  for( size_t i = 0; i < n; ++i ) {
    addR( Vector3D(2*sqr,0,0), spd );
    addR( Vector3D(0,-sqr,0), spd );
    addR( Vector3D(-2*sqr,0,0), spd );
    addR( Vector3D(0,-sqr,0), spd );
    addR( Vector3D(5*sqr,0,0), spd );
    addR( Vector3D(0,sqr,0), spd );
    addR( Vector3D(-2*sqr,0,0), spd );
    addR( Vector3D(0,sqr,0), spd );
    addR( Vector3D(3*sqr,0,0), spd );
  }
}

//-----------------------------------------------------------------------------
Vector3I BricksTrajectory::initialOffset() const {
  int sig = rotation_vec_.x() / fabs(rotation_vec_.x());
  return rotate( Vector3D(offset_.x()*sig, offset_.y(), offset_.z()) );
}

//-----------------------------------------------------------------------------
void BricksTrajectory::draft( PositionVector &out, double spd, double ampl ) {
  BricksTrajectory e( spd, ampl, Vector3D(9*ampl,0,0), 0);
  out.clear();
  out.push_back( e.initialOffset() );
  out.insert( out.end(), e.positions_.begin(), e.positions_.end() );
}

//-----------------------------------------------------------------------------
