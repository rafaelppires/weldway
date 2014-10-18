#include <linear_transform.h>
#include <geometry.h>

LinearTransform::LinearTransform(const Vector3D &r, double angle) : rotation_(r), xangle_(angle) {
  setupMatrixes();
}

//-----------------------------------------------------------------------------
void LinearTransform::setupMatrixes() {
  Vector2D xzproj( rotation_.x(), rotation_.z() );
  long double alpha = xangle_,
              beta = atan2( xzproj.y(), xzproj.x() ),
              gama = atan2( rotation_.y(), xzproj.length() );

  MatrixD overx(rotationMatrix3D(0,  alpha)),
          overy(rotationMatrix3D(1, -beta)),
          overz(rotationMatrix3D(2, -gama));

  rotation_matrix_ = overz * overy * overx;
}

//-----------------------------------------------------------------------------
Vector3D LinearTransform::transform(const Vector3D &v) {
  MatrixD rotated = rotation_matrix_ * v.columnMatrix();
  return Vector3D( column2vec(rotated) * (rotation_.x() < 0?-1:1) );
}

//-----------------------------------------------------------------------------
Vector3D LinearTransform::revert(const Vector3D &v) {
  MatrixD rotated = v.lineMatrix() * rotation_matrix_;
  return Vector3D( line2vec(rotated) );
}

//-----------------------------------------------------------------------------
double LinearTransform::length() {
  return rotation_.length();
}
