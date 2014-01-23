#ifndef _DOUBLE_E_H_
#define _DOUBLE_E_H_

#include <trajectory.h>
#include <stdint.h>

class DoubleETrajectory : public AbstractTrajectory {
public:
  DoubleETrajectory(int32_t spd, double freq, int32_t ampl, double total_length);
private:
  typedef Vector2D Coordinate;
  void add( const Coordinate& ) {}
  double xsteplen_, torch_speed_, amplitude_;
};

#endif
