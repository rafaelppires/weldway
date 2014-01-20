#ifndef _DOUBLE_E_H_
#define _DOUBLE_E_H_

#include <trajectory.h>
#include <stdint.h>
#include <matrix_trajectory.h>

class DoubleETrajectory : public MatrixTrajectory {
public:
  DoubleETrajectory(int32_t spd, double freq, int32_t ampl, double total_length);
private:
};

#endif
