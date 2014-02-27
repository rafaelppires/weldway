#ifndef _E_TRAJECTORY_H_
#define _E_TRAJECTORY_H_

#include <trajectory.h>
#include <stdint.h>

class ETrajectory : public AbstractTrajectory {
public:
  ETrajectory(double spd, double l, double ampl,  const Vector3D &rotate_vec, double deg_xang );
  Vector3I initialOffset() const;
  void applyCorrection( double spd, double l, double ampl );
  void addRepeatable( uint16_t count, double l, double yoff, double vr );

private:
  Vector3D offset_;
};

#endif
