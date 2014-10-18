#include <circular_transform.h>
#include <geometry.h>

//-----------------------------------------------------------------------------
CircularTransform::CircularTransform(const Vector3D &center, const Vector3D &beginning, double xangle, double angle_range, const Vector2D &torch) :
  r_( center.distance(beginning) ), arange_(angle_range), rotation_(rotationMatrix3D(0,xangle)),
  circular_( center.distance(beginning) ) {
  Vector3D xaxis(1,0,0), beg(beginning - center),
           projection( xaxis * xaxis.dot(beg) );
  a0_ = atan2( beg.y(), projection.x() );

  std::cout << "a0 = " << (a0_*180/3.1415926535) << " xangle = " << (xangle*180/3.1415926535) << "\n";


  double va = torch.x(), ha = torch.y();
  Vector3D z(0,0,1);
  MatrixD overy(rotationMatrix3D(1,ha));
  MatrixD prevx = Vector3D(1,0,0).lineMatrix() * overy;
  MatrixD overprevx = rotationMatrix3D(line2vec(prevx),va);

  Vector3D rotated = column2vec(overprevx*overy*z.columnMatrix());
  std::cout << rotated << "\n";

  Vector3D unrot1 = column2vec( rotationMatrix3D(0,-atan2(rotated.z(),rotated.y())) * rotated.columnMatrix() );
  std::cout << unrot1 << "\n";

}

//-----------------------------------------------------------------------------
Vector3D CircularTransform::transform(const Vector3D &v) {
  PositionVector pv;
  MatrixD rotated = rotation_ * v.columnMatrix();
  pv.push_back( column2vec(rotated) );
  pv = CurveTrajectory::process( pv, circular_, a0_, arange_/fabs(arange_) );
  return pv.front();
}

//-----------------------------------------------------------------------------
Vector3D CircularTransform::revert(const Vector3D &v) {
  return v;
}

//-----------------------------------------------------------------------------
double CircularTransform::length() {
  return fabs(r_*arange_);
}

//-----------------------------------------------------------------------------
