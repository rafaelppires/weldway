#ifndef BRICKS_H
#define BRICKS_H

#include <trajectory.h>
#include <stdint.h>

class BricksTrajectory : public AbstractTrajectory {
public:
  BricksTrajectory( double spd, double ampl, TrajectoryTransformPtr tt );
  void applyCorrection( double spd, double ampl );
  static void draft( PositionVector &out, double spd, double ampl );
  Vector3I initialOffset() const;
private:
  void addRepeatable( size_t n, double sqr, double spd );
  Vector3D offset_;
};

#endif // BRICKS_H
