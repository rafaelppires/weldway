#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <cmath>
#include <Matrix.h>
#include <vectorxd.h>

const long double pi = acosl(-1);

//-----------------------------------------------------------------------------
static inline double rad2deg(double angle) { return angle * 180 / pi; }
static inline double deg2rad(double angle) { return angle * pi / 180; }
static inline Vector3D line2vec(const MatrixD &m)   { return Vector3D(m(0,0),m(0,1),m(0,2)); }
static inline Vector3D column2vec(const MatrixD &m) { return Vector3D(m(0,0),m(1,0),m(2,0)); }
MatrixD rotationMatrix3D(int axis, double angle);
MatrixD rotationMatrix3D(const Vector3D &v, double angle);

//-----------------------------------------------------------------------------

#endif // GEOMETRY_H
