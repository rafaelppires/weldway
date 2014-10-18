#include <custom_trajectory.h>
#include <iostream>

//-----------------------------------------------------------------------------
void CustomTrajectory::addSegment( const Vector2D &p, double spd, double fnt) {
  segments_.push_back( CustomEntry(p,spd,fnt) );
}

//-----------------------------------------------------------------------------
void CustomTrajectory::split( int idx ) {
  if( idx < 0 || idx >= segments_.size() ) return;
  CustomEntry &e1 = segments_[idx], e2;
  e1.point = e1.point / 2;
  e2 = e1;
  segments_.insert( segments_.begin()+idx+1, e2 );
}
//-----------------------------------------------------------------------------
Vector2D CustomTrajectory::remove( int idx ) {
  if( segments_.size() < 2 || idx < 0 || idx >= segments_.size() ) return Vector2D(1,1);
  if( idx == 0 ) segments_[1].point += segments_[0].point;
  else segments_[idx-1].point += segments_[idx].point;
  segments_.erase( segments_.begin()+idx );
  return rescale();
}

//-----------------------------------------------------------------------------
Vector2D CustomTrajectory::moveSource( int idx, const Vector2D &d ) {
  if( idx < 0 || idx >= segments_.size() ) return Vector2D(1,1);
  if( idx - 1 >= 0 ) segments_[idx-1].point += d;
  segments_[idx].point -= d;
  return rescale();
}

//-----------------------------------------------------------------------------
Vector2D CustomTrajectory::moveDestination(int idx, const Vector2D &d ) {
  if( idx < 0 || idx >= segments_.size() ) return Vector2D(1,1);
  if( idx + 1 < segments_.size() ) segments_[idx+1].point -= d;
  segments_[idx].point += d;
  return rescale();
}

//-----------------------------------------------------------------------------
Vector2D CustomTrajectory::rescale() {
  SegmentVector::iterator it = segments_.begin(), end = segments_.end();
  Vector2D cur(0,0), mm(1e+10,1e-10);
  for(;it!=end;++it){
    cur += it->point;
    if( cur.y() > mm.y() ) mm.y() = cur.y();
    if( cur.y() < mm.x() ) mm.x() = cur.y();
  }

  double sy = fabs(mm.y()-mm.x());
  Vector2D scale( cur.x(), sy < 1e-8 ? 1 : sy );
  for(it = segments_.begin();it!=end;++it){
    it->point.x() /= scale.x();
    it->point.y() /= scale.y();
  }
  return scale;
}

//-----------------------------------------------------------------------------
void CustomTrajectory::draft(PositionVector &v, double,double lmbd,double ampl) {
  Vector3D pos(0,0,0);
  v.push_back(pos);
  for(int i = 0; i < 4; ++i) {
    SegmentVector::iterator it = segments_.begin(), end = segments_.end();
    for(; it!=end;++it) {
      pos += Vector3D( it->point.x()*lmbd, -it->point.y()*ampl, 0 );
      v.push_back( pos );
    }
  }
}

//-----------------------------------------------------------------------------
AbsTrajectoryPtr CustomTrajectory::getExecutable(TrajectoryTransformPtr tt, double spd, double lmbd, double ampl ) {
  return AbsTrajectoryPtr( new CustomExecutable(tt, spd, lmbd, ampl, segments_ ) );
}

//-----------------------------------------------------------------------------
CustomExecutable::CustomExecutable(TrajectoryTransformPtr tt,
                                   double s, double l, double a,
                                   const SegmentVector &svec) :
  AbstractTrajectory(tt) {

  double len, prd = (fabs(l) * TO_RPM) / (fabs(s) * TO_PULSES);
  SegmentVector::const_iterator it = svec.begin(), end = svec.end();
  for(; it!=end;++it)
    len += Vector2D( it->point.x()*l, -it->point.y()*a ).length();
  double spdmm = len/(prd*TO_PULSES);


  int period_count = 0.5 + tt->length()/l;
  for(int i = 0; i < period_count; ++i) {
    SegmentVector::const_iterator it = svec.begin(), end = svec.end();
    for(; it!=end;++it) {
      addR( Vector3D( it->point.x()*l, -it->point.y()*a, 0 ), spdmm );
    }
  }
  rotate();
}
//-----------------------------------------------------------------------------
