#include <bricks.h>
#include <linear_transform.h>
//-----------------------------------------------------------------------------
BricksTrajectory::BricksTrajectory( double spd, double ampl,
                                    TrajectoryTransformPtr tt) :
    AbstractTrajectory(tt) {

  double sqsz = ampl / 2, lmbd = 6*sqsz, total = tt->length();
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
  int sig = 1; // CHECK rotation_vec_.x() / fabs(rotation_vec_.x());
  return transform_->transform( Vector3D(offset_.x()*sig, offset_.y(), offset_.z()) );
}

//-----------------------------------------------------------------------------
void BricksTrajectory::draft( PositionVector &out, double spd, double ampl ) {
  BricksTrajectory e( spd, ampl, TrajectoryTransformPtr(new LinearTransform(Vector3D(9*ampl,0,0), 0)));
  out.clear();
  out.push_back( e.initialOffset() );
  out.insert( out.end(), e.positions_.begin(), e.positions_.end() );
}

//-----------------------------------------------------------------------------
