#ifndef DOUBLE_TRIANG_H
#define DOUBLE_TRIANG_H

#include <trajectory.h>
#include <stdint.h>

class DoubleTriangularTraj : public AbstractTrajectory {
public:
  DoubleTriangularTraj(double spd, double lmbd, double ampl,
                       TrajectoryTransformPtr tt);

  void applyCorrection(double spd, double lmbd, double ampl);

  void addRepeatable(int count, double lmbd, double ampl, double spd);
  static void draft( PositionVector &out, double spd, double l, double ampl );
};


#endif // DOUBLE_TRIANG_H
