#ifndef CIRCULAR_TRANSFORM_H
#define CIRCULAR_TRANSFORM_H

#include <trajectory_transform.h>
#include <curved_trajectory.h>
#include <queue>

class CircularTransform : public TrajectoryTransform {
public:
  CircularTransform(const Vector3D &center, const Vector3D &beginning, double xangle, double angle_range, const Vector2D &torch);
  virtual Vector3D transform(const Vector3D &v);
  virtual Vector3D revert(const Vector3D &v);
  virtual double length();
  virtual bool controlsTorch();
  virtual Vector2D gettorch();

private:
  Circular circular_;
  double r_, arange_, a0_;
  MatrixD rotation_;
  Vector3D torch_dir_;
  std::queue<Vector2D> torch_;
};

#endif // CIRCULAR_TRANSFORM_H
