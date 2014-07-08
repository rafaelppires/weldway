#ifndef _TRIANGULAR_H_
#define _TRIANGULAR_H_

#include <trajectory.h>
#include <stdint.h>

class TriangularTrajectory : public AbstractTrajectory {
public:
  TriangularTrajectory(double spd, double lmbd, double ampl,
                       double sstop, double istop, double f, const Vector3D &rotate_vec, double deg_xang );

  void applyCorrection(double spd, double lmbd, double ampl,
                       uint32_t sstop, uint32_t istop);

  void addRepeatable(int count, double lmbd, double dsup, double dinf,
                     double ampl, double vint, double vext );
  static void draft(PositionVector &out, double spd, double l, double ampl, double sstop, double istop , double f);
};

#endif
