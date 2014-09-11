#include <custom_trajectory.h>

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
