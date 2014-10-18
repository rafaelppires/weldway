#ifndef LINEAR_TRANSFORM_H
#define LINEAR_TRANSFORM_H

#include <trajectory_transform.h>

class LinearTransform : public TrajectoryTransform {
public:
  LinearTransform(const Vector3D &r, double angle);
  virtual Vector3D transform(const Vector3D &v);
  virtual Vector3D revert(const Vector3D &v);
  virtual double length();

private:
  void setupMatrixes();

  Vector3D rotation_;
  double xangle_;
  MatrixD rotation_matrix_;
};

#endif // LINEAR_TRANSFORM_H
