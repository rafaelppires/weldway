#ifndef CIRCULAR_TRANSFORM_H
#define CIRCULAR_TRANSFORM_H

#include <trajectory_transform.h>
#include <curved_trajectory.h>

class CircularTransform : public TrajectoryTransform {
public:
  CircularTransform(const Vector3D &center, const Vector3D &beginning, double xangle, double angle_range, const Vector2D &torch);
  virtual Vector3D transform(const Vector3D &v);
  virtual Vector3D revert(const Vector3D &v);
  virtual double length();

private:
  Circular circular_;
  double r_, arange_, a0_;
  MatrixD rotation_;
};

#endif // CIRCULAR_TRANSFORM_H
