#include <geometry.h>

//-----------------------------------------------------------------------------
MatrixD rotationMatrix3D(int axis, double angle) {
  MatrixD ret(3);
  if( axis < 0 || axis > 2 ) return ret;
  int next = (4 + axis)%3,
      prev = (2 + axis)%3, min = std::min(next,prev), max = std::max(next,prev);
  ret(min,min) = ret(max,max) = cos(angle);
  ret(min,max) = sin(angle);
  ret(max,min) = -ret(min,max);
  return ret;
}

//-----------------------------------------------------------------------------
MatrixD rotationMatrix3D(const Vector3D &v, double angle) {
  MatrixD ret(3);
  Vector3D axis = v.unary();
  double c = cos(angle), s = sin(angle);
  ret(0,0) = c+axis.x()*axis.x()*(1-c);
  ret(1,1) = c+axis.y()*axis.y()*(1-c);
  ret(2,2) = c+axis.z()*axis.z()*(1-c);

  ret(0,1) = ret(0,1) = axis.x()*axis.y()*(1-c);
  ret(0,1) -= axis.z()*s;
  ret(1,0) += axis.z()*s;

  ret(0,2) = ret(2,0) = axis.x()*axis.z()*(1-c);
  ret(0,2) += axis.y()*s;
  ret(2,0) -= axis.y()*s;

  ret(1,2) = ret(2,1) = axis.z()*axis.y()*(1-c);
  ret(1,2) -= axis.x()*s;
  ret(2,1) += axis.x()*s;

  return ret;
}

//-----------------------------------------------------------------------------
