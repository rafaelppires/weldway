#ifndef _RHOMBUS_H_
#define _RHOMBUS_H_

#include <matrix_trajectory.h>
#include <vector2d.h>

class Rhombus : public MatrixTrajectory {
public:
  Rhombus(int32_t a, int32_t l, uint8_t n, double wspeed);
};

#endif
