#include <double_e.h>
#include <linear_transform.h>

//-----------------------------------------------------------------------------
DoubleETrajectory::DoubleETrajectory( double spd, double lambda, int32_t ampl,
                                      TrajectoryTransformPtr tt ) :
  AbstractTrajectory(tt) {
  double total_length = tt->length();
  int period_count = 0.5 + total_length/lambda;
  double freq = (TO_PULSES*spd) / (TO_RPM*lambda),
         t = 1. / freq,                      // s
         hsqr2 = sqrt(2.) / 2.,
         l = lambda/2.,                      // pulses
         k = ampl / (4.*l*hsqr2),            // nondimensional
         d = sqrt(l*l*(k*k+1.)) / 2.,
         yoff = k*l*hsqr2,
         vr = (8. * d + 2. * l) / (TO_PULSES * t); // mm / s
  addRepeatable( period_count, lambda, yoff, vr );
  rotate();
}

//-----------------------------------------------------------------------------
void DoubleETrajectory::applyCorrection( double spd, double lambda,
                                         int32_t ampl ) {
  boost::lock_guard<boost::mutex> lock(data_mutex_);
  if( index_ < 2 || index_ > positions_.size() - 2 ) return;
  Vector3D diff, cur, prev;
  uint32_t idx = index_;
  double prevdiffy;

  cur = transform_->revert( positions_[idx-1] );
  do {
    prev = cur;
    cur = transform_->revert( positions_[idx++] );
    prevdiffy = diff.y();
    diff = cur - prev;
    if( fabs(diff.x())<1e-5 ) diff.x() = 0;
    if( fabs(diff.y())<1e-5 ) diff.y() = 0;
  } while( idx < positions_.size() && (diff.y() != 0 || prevdiffy <= 0) );
  eraseFrom( idx-1 );
  int period_count = 0.5 + (transform_->length() - cur.x())/lambda;
  double freq = (TO_PULSES*spd) / (TO_RPM*lambda),
         t = 1. / freq,                      // s
         hsqr2 = sqrt(2.) / 2.,
         l = lambda/2.,                      // pulses
         k = ampl / (4.*l*hsqr2),            // nondimensional
         d = sqrt(l*l*(k*k+1.)) / 2.,
         yoff = k*l*hsqr2,
         vr = (8. * d + 2. * l) / (TO_PULSES * t); // mm / s
  PositionVector::reference last = positions_.back();
  last = prev;
  setReference();
  addRepeatable( period_count, lambda, yoff, vr);
  for( int i = idx-2; i < positions_.size(); ++i )
    positions_[i] = transform_->transform(positions_[i]);
}

//-----------------------------------------------------------------------------
void DoubleETrajectory::addRepeatable( uint32_t period_count, double lmb,
                                       double yoff, double spd ) {
  double l = lmb / 2.,
         xoff = l * sqrt(2.)/2.;
  for( uint32_t i = 0; i < period_count; ++i ) {
    addR( Vector3D(     l,     0, 0 ), spd );
    addR( Vector3D(  xoff,  yoff, 0 ), spd );
    addR( Vector3D( -xoff,  yoff, 0 ), spd );
    addR( Vector3D( -xoff, -yoff, 0 ), spd );
    addR( Vector3D(  xoff, -yoff, 0 ), spd );

    addR( Vector3D(     l,     0, 0 ), spd );
    addR( Vector3D(  xoff, -yoff, 0 ), spd );
    addR( Vector3D( -xoff, -yoff, 0 ), spd );
    addR( Vector3D( -xoff,  yoff, 0 ), spd );
    addR( Vector3D(  xoff,  yoff, 0 ), spd );
  }
}
//-----------------------------------------------------------------------------
void DoubleETrajectory::draft( PositionVector &out, double spd, double l, double ampl ) {
  DoubleETrajectory e( spd, l, ampl, TrajectoryTransformPtr( new LinearTransform(Vector3D(4*l,0,0), 0)));
  out.clear();
  out.push_back( e.initialOffset() );
  out.insert( out.end(), e.positions_.begin(), e.positions_.end() );
}

//-----------------------------------------------------------------------------
