#ifndef LINEAR_TRANSFORM_H
#define LINEAR_TRANSFORM_H

#include <trajectory_transform.h>

class LinearTransform : public TrajectoryTransform {
public:
  virtual Vector3D transform(const Vector3D &v);
  virtual Vector3D revert(const Vector3D &v);
};

#endif // LINEAR_TRANSFORM_H
