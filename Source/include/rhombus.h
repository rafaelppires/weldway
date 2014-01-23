#ifndef _RHOMBUS_H_
#define _RHOMBUS_H_

#include <trajectory.h>
#include <vectorxd.h>

class Rhombus : public AbstractTrajectory {
public:
  Rhombus(int32_t a, int32_t l, uint8_t n, double wspeed, double total_length);
private:
  typedef Vector2D Coordinate;
  void add( const Coordinate& ) {}
  double xsteplen_, torch_speed_, amplitude_;
  bool yalternate_;
};

#endif
