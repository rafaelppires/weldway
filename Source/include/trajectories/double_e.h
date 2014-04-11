#ifndef _DOUBLE_E_H_
#define _DOUBLE_E_H_

#include <trajectory.h>
#include <stdint.h>

class DoubleETrajectory : public AbstractTrajectory {
public:
  DoubleETrajectory( double spd, double lambda, int32_t ampl, const Vector3D &rotate_vec, double deg_xang );
  void applyCorrection( double spd, double lambda, int32_t ampl );
  static void draft( PositionVector &out, double spd, double l, double ampl );
private:
  void addRepeatable( uint32_t period_count, double lmb, double yoff, double spd );
};

#endif
