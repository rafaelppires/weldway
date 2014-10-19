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
  std::cout << "va: " << va << " ha: " << ha << "\n";
  Vector3D z(0,0,1);
  MatrixD overy(rotationMatrix3D(1,ha));
  MatrixD prevx = Vector3D(1,0,0).lineMatrix() * overy;
  MatrixD overprevx = rotationMatrix3D(line2vec(prevx),va);
  std::cout << "prevx " << prevx << "\nMatrix: \n" << overprevx << "\n";

  torch_dir_ = column2vec(overprevx*overy*z.columnMatrix());
  std::cout << "Rotated: " << torch_dir_ << "\n";

  double vangle, hangle;
  MatrixD un_overx( rotationMatrix3D(0,vangle=-atan2(torch_dir_.y(),torch_dir_.z())) );
  Vector3D unrot1 = column2vec( un_overx * torch_dir_.columnMatrix() );
  hangle=-atan2( unrot1.x(), unrot1.z() );
  std::cout << "Angles: v: " << rad2deg(vangle) << " h: " << rad2deg(-hangle) << "\n";
}

//-----------------------------------------------------------------------------
Vector3D CircularTransform::transform(const Vector3D &v) {
  PositionVector pv;
  int sig = arange_/fabs(arange_);
  MatrixD rotated = rotation_ * v.columnMatrix();
  Vector3D cur = column2vec(rotated);
  pv.push_back( cur );
  pv = CurveTrajectory::process( pv, circular_, a0_, sig );

  double angle = circular_.invarclen( cur.x() );
  Vector3D cur_torch = column2vec( rotationMatrix3D(2,angle) * torch_dir_.columnMatrix() );
  double vangle, hangle;
  MatrixD un_overx( rotationMatrix3D(0,vangle=-atan2(cur_torch.y(),cur_torch.z())) );
  Vector3D unrot1 = column2vec( un_overx * cur_torch.columnMatrix() );
  hangle=-atan2( unrot1.x(), unrot1.z() );
  std::cout << "Angles: v: " << rad2deg(vangle) << " h: " << rad2deg(-hangle) << "\n";
  torch_.push( Vector2D(vangle,-hangle) );

  return pv.front();
}

//-----------------------------------------------------------------------------
Vector2D CircularTransform::gettorch() {
  Vector2D ret = torch_.front();
  ret.x() = rad2deg( ret.x() );
  ret.y() = rad2deg( ret.y() );
  torch_.pop();
  return ret;
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
bool CircularTransform::controlsTorch() {
  return true;
}

//-----------------------------------------------------------------------------
