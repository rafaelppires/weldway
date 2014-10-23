#include <circular_transform.h>
#include <geometry.h>

//-----------------------------------------------------------------------------
CircularTransform::CircularTransform(const Vector3D &center, const Vector3D &beginning, double xangle, double angle_range, const Vector2D &torch) :
  r_( center.distance(beginning) ), arange_(angle_range), rotation_(rotationMatrix3D(0,xangle)),
  circular_( center.distance(beginning) ) {
  Vector3D xaxis(1,0,0), beg(beginning - center),
           projection( xaxis * xaxis.dot(beg) );
  a0_ = atan2( beg.y(), projection.x() );

  //std::cout << "a0 = " << rad2deg(a0_) << " xangle = " << rad2deg(xangle) << "\n";


  double va = torch.x(), ha = torch.y();
  //std::cout << "va: " << va << " (" << rad2deg(va) << ") ha: " << ha << " (" << rad2deg(ha) << ")\n";
  Vector3D z(0,0,1);
  torch_dir_ = column2vec( rotationMatrix3D(1,ha) * rotationMatrix3D(0,va) * z.columnMatrix());
  //std::cout << "Rotated: " << torch_dir_ << "\n";
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
  MatrixD un_overy( rotationMatrix3D(1,hangle=-atan2(cur_torch.x(),cur_torch.z())) );
  Vector3D unrot1 = column2vec( un_overy * cur_torch.columnMatrix() );
  vangle=atan2( unrot1.y(), unrot1.z() );
  //std::cout << "Angles: v: " << rad2deg(vangle) << " h: " << rad2deg(-hangle) << "\n";
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
