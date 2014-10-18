#ifndef TRAJECTORY_TRANSFORM_H
#define TRAJECTORY_TRANSFORM_H

#include <vectorxd.h>

class TrajectoryTransform {
public:
  virtual Vector3D transform(const Vector3D &v) = 0;
  virtual Vector3D revert(const Vector3D &v) = 0;
  virtual double length() = 0;
};

typedef boost::shared_ptr<TrajectoryTransform> TrajectoryTransformPtr;

#endif // TRAJECTORY_TRANSFORM_H
