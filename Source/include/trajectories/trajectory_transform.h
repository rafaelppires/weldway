#ifndef TRAJECTORY_TRANSFORM_H
#define TRAJECTORY_TRANSFORM_H

#include <vectorxd.h>

class TrajectoryTransform {
public:
  virtual Vector3D transform(const Vector3D &v) = 0;
  virtual Vector3D revert(const Vector3D &v) = 0;
  virtual bool controlsTorch() = 0;
  virtual double length() = 0;
  virtual Vector2D gettorch() { return Vector2D(0,0); }
};

typedef boost::shared_ptr<TrajectoryTransform> TrajectoryTransformPtr;

#endif // TRAJECTORY_TRANSFORM_H
