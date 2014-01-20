#ifndef _TRIANGULAR_H_
#define _TRIANGULAR_H_

#include <trajectory.h>
#include <stdint.h>

class TriangularTrajectory : public AbstractTrajectory {
public:
  TriangularTrajectory(int32_t, double, int32_t, uint32_t, uint32_t , double total_length);
private:
  void add( Vector3I &accumulator, const Vector3I &delta );
};

#endif
