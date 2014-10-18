#ifndef _DOUBLE8_H_
#define _DOUBLE8_H_

#include <trajectory.h>
#include <stdint.h>

class Double8Trajectory : public AbstractTrajectory {
public:
  Double8Trajectory(double spd, double lambda, double ampl, double rho, TrajectoryTransformPtr tt);
  void applyCorrection( double spd, double lambda, double ampl, double rho );
  static void draft( PositionVector &out, double spd, double l, double ampl, double rho );
private:
  void addRepeatable( uint32_t period_count );
  PositionVector psingle_;
  SpeedVector ssingle_;
};

#endif
