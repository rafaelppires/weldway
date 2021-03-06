#include <e_trajectory.h>
#include <linear_transform.h>

//-----------------------------------------------------------------------------
ETrajectory::ETrajectory( double spd, double l, double ampl, double rho,
                          TrajectoryTransformPtr tt ) :
        AbstractTrajectory(tt), l_(l), rho_(rho), A_(ampl)  {
  double total_length = tt->length();
  int period_count = 0.5 + total_length/l;

  defineSingle( spd, l );
  addRepeatable( period_count );
  rotate();
}

//-----------------------------------------------------------------------------
void ETrajectory::defineSingle( double spd, double l ) {
  psingle_.clear();
  ssingle_.clear();

  double pi = acos(-1), arc = 3*pi/2,
         domain = 7 * pi / 2. + 0.01,
         delta = pi / 4, ulen = 0, llen = 0;
  Vector3D off(-f(3*pi).x(),0,0), prev, cur;
  prev = offset_ = f(arc) + off;
  setReference( offset_ );
  for( arc += delta; arc < domain; arc += delta ) {
    cur = f(arc) + off;
    Vector3D p = cur - prev;
    psingle_.push_back( p );
    if( fabs(arc-2*pi) > 1e-5 && arc > 2*pi &&      // > 2*pi
        (arc < 3 * pi || fabs(arc-3*pi) < 1e-5) ) { // <= 3*pi
      ssingle_.push_back( 1 );
      ulen += p.length();
    } else {
      ssingle_.push_back( -1 );
      llen += p.length();
    }
    prev = cur;
  }

  double hper = (l*TO_RPM) / (2*spd*TO_PULSES), // half period in s
         uspd = (ulen) / (TO_PULSES*hper),      // upper spd in mm/s
         lspd = (llen) / (TO_PULSES*hper);      // lower spd in mm/s
  SpeedVector::iterator it = ssingle_.begin(), end = ssingle_.end();
  for(;it!=end;++it ) *it = *it < 0 ? lspd : uspd;
}

//-----------------------------------------------------------------------------
Vector3D ETrajectory::f( double arc ) {
  double pi = acos(-1);
  return Vector3D( l_*(arc/(2*pi)+(3-rho_)*cos(arc)/(4-4*rho_)),
                   A_ * sin(arc) / 2, 0);
}

//-----------------------------------------------------------------------------
Vector3I ETrajectory::initialOffset() const {
  int sig = 1; //CHECK rotation_vec_.x() / fabs(rotation_vec_.x());
  return transform_->transform( Vector3D(offset_.x()*sig, offset_.y(), offset_.z()) );
}

//-----------------------------------------------------------------------------
void ETrajectory::applyCorrection( double spd, double l, double ampl, double rho ) {
  boost::lock_guard<boost::mutex> lock(data_mutex_);
  if( index_ < 1 || index_ > positions_.size() - 2 ) return;
  Vector3D diff, cur, prev;
  uint32_t idx = index_;

  prev = transform_->transform( positions_[idx-1] );
  do {
    cur = transform_->revert( positions_[idx++] );
    diff = cur - prev;
    prev = cur;
  } while( idx < positions_.size() && (diff.y() > 0 || Vector3D(transform_->revert(positions_[idx])-prev).y() < 0) );

  eraseFrom( idx );
  A_ = ampl;
  rho_ = rho;
  defineSingle( spd, l );

  positions_.back().y() = 0;
  positions_.back() += offset_;
  setReference();

  int period_count = 0.5 + (transform_->length() - positions_[idx].x())/l;
  addRepeatable( period_count );
  for( int i = idx; i < positions_.size(); ++i )
    positions_[i] = transform_->transform(positions_[i]);
}

//-----------------------------------------------------------------------------
void ETrajectory::addRepeatable( uint16_t count ) {
  for( uint32_t i = 0; i < count; ++i ) {
    size_t sz = psingle_.size();
    for( size_t j = 0; j < sz; ++j ) {
      addR( psingle_[j], ssingle_[j] );
    }
  }
}

//-----------------------------------------------------------------------------
void ETrajectory::draft( PositionVector &out, double spd, double l, double ampl, double rho ) {
  ETrajectory e( spd, l, ampl, rho, TrajectoryTransformPtr(new LinearTransform(Vector3D(4*l,0,0), 0)));
  out.clear();
  out.push_back( e.initialOffset() );
  out.insert( out.end(), e.positions_.begin(), e.positions_.end() );
}

//-----------------------------------------------------------------------------
void ETrajectory::getSingle( PositionVector&p, SpeedVector&s ) {
  p = psingle_; s = ssingle_;
}

//-----------------------------------------------------------------------------
