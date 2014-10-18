#ifndef DOUBLE_TRIANG2NDVERSION_H
#define DOUBLE_TRIANG2NDVERSION_H

#include <trajectory.h>
#include <stdint.h>

class DoubleTriangular2ndTraj : public AbstractTrajectory {
public:
  DoubleTriangular2ndTraj(double spd, double lmbd, double ampl,
                       TrajectoryTransformPtr tt );

  void applyCorrection(double spd, double lmbd, double ampl);

  void addRepeatable(int count, double lmbd, double ampl, double spd);
  static void draft( PositionVector &out, double spd, double l, double ampl );
};

#endif // DOUBLE_TRIANG2NDVERSION_H
