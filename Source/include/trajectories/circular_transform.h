#ifndef CIRCULAR_TRANSFORM_H
#define CIRCULAR_TRANSFORM_H

#include <trajectory_transform.h>

class CircularTransform : public TrajectoryTransform {
public:
  virtual Vector3D transform(const Vector3D &v);
  virtual Vector3D revert(const Vector3D &v);
};

#endif // CIRCULAR_TRANSFORM_H
