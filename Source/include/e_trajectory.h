#ifndef _E_TRAJECTORY_H_
#define _E_TRAJECTORY_H_

#include <trajectory.h>
#include <stdint.h>

class ETrajectory : public AbstractTrajectory {
public:
  ETrajectory(int32_t spd, double freq, int32_t ampl, double total_length);
private:
  typedef Vector2D Coordinate;
  void add( const Coordinate& ) {}
  double xsteplen_, torch_speed_, amplitude_;
};

#endif
