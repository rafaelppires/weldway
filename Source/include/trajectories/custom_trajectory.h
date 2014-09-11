#ifndef CUSTOM_TRAJECTORY_H
#define CUSTOM_TRAJECTORY_H

#include <trajectory.h>

//-----------------------------------------------------------------------------
struct CustomEntry {
  CustomEntry(){}
  CustomEntry( const Vector2D &p, double &s, double &f ) : point(p), speed(s), font(f) {}
  Vector2D point;
  double speed, font;
};
//-----------------------------------------------------------------------------
typedef std::vector< CustomEntry > SegmentVector;
//-----------------------------------------------------------------------------
class CustomTrajectory : public AbstractTrajectory {
public:
  void addSegment( const Vector2D &p, double spd, double fnt);
  void split( int );
  const SegmentVector& getSegments() { return segments_; }
private:
  SegmentVector segments_;
};

//-----------------------------------------------------------------------------

#endif // CUSTOM_TRAJECTORY_H
