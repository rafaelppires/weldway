#ifndef _TRIANGULAR_H_
#define _TRIANGULAR_H_

#include <trajectory.h>
#include <stdint.h>

class TriangularTrajectory : public AbstractTrajectory {
public:
  TriangularTrajectory(double spd, double lmbd, double ampl,
                       uint32_t sstop, uint32_t istop, const Vector3D &rotate_vec, double deg_xang );

  void applyCorrection(double spd, double lmbd, double ampl,
                       uint32_t sstop, uint32_t istop);

  void addRepeatable(int count, uint32_t sstop, uint32_t istop,
                      double risexlen, double ampl, double risespd, double xspeedmm );
};

#endif
