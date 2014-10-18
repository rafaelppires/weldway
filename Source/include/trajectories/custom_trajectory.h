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
class CustomExecutable : public AbstractTrajectory {
public:
  CustomExecutable(TrajectoryTransformPtr tt, double s, double l, double a, const SegmentVector &svec);
};
//-----------------------------------------------------------------------------
class CustomTrajectory {
public:
  CustomTrajectory() : name_("Sem nome") {}
  void addSegment( const Vector2D &p, double spd, double fnt);
  void split( int );
  Vector2D remove( int );
  Vector2D moveSource(int idx, const Vector2D &d );
  Vector2D moveDestination(int idx, const Vector2D &d );
  const SegmentVector& getSegments() { return segments_; }
  void setSegments( const SegmentVector &s ) { segments_ = s; }
  void setName( std::string n ) { name_ = n; }
  std::string name() { return name_; }
  void draft(PositionVector &v,double,double,double);
  AbsTrajectoryPtr getExecutable(TrajectoryTransformPtr tt, double spd, double lmbd, double ampl );
private:
  std::string name_;
  Vector2D rescale();
  SegmentVector segments_;
};
//-----------------------------------------------------------------------------
typedef boost::shared_ptr<CustomTrajectory> CustomTrajectoryPtr;
typedef std::vector<CustomTrajectoryPtr> CustomTrajVector;
//-----------------------------------------------------------------------------

#endif // CUSTOM_TRAJECTORY_H
