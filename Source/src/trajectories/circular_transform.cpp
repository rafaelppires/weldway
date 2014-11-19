#include <circular_transform.h>
#include <geometry.h>
#include <fstream>

//-----------------------------------------------------------------------------
CircularTransform::CircularTransform(const Vector3D &center, const Vector3D &beginning, double xangle, double angle_range, const Vector2D &torch, double offset) :
  r_( center.distance(beginning) ), arange_(angle_range), rotation_(rotationMatrix3D(0,xangle)),
  circular_( center.distance(beginning) ), offset_(offset) {
  Vector3D xaxis(1,0,0), beg(beginning - center),
           projection( xaxis * xaxis.dot(beg) );
  a0_ = atan2( beg.y(), projection.x() );

  //std::cout << "a0 = " << rad2deg(a0_) << " xangle = " << rad2deg(xangle) << "\n";


  double va = torch.x(), ha = torch.y();
  //std::cout << "va: " << va << " (" << rad2deg(va) << ") ha: " << ha << " (" << rad2deg(ha) << ")\n";
  Vector3D z(0,0,-270*40.);
  MatrixD initrot = rotationMatrix3D(1,ha) * rotationMatrix3D(0,va);
  torch_dir_ = column2vec( initrot * z.columnMatrix() );
  tip_init_  = column2vec( initrot * Vector3D(0,76*40.,0).columnMatrix() ) + torch_dir_;
}
//-----------------------------------------------------------------------------
std::ofstream circ("circ.dat");
Vector3D CircularTransform::transform(const Vector3D &v) {
  PositionVector pv;
  int sig = arange_/fabs(arange_);
  MatrixD rotated = rotation_ * v.columnMatrix();
  Vector3D cur = column2vec(rotated);
  pv.push_back( cur );
  pv = CurveTrajectory::process( pv, circular_, a0_, sig );

  double angle = circular_.invarclen( cur.x() );
  MatrixD curangle_transform = rotationMatrix3D(2,angle);

  Vector3D cur_torch = column2vec( curangle_transform * torch_dir_.columnMatrix() );
  double vangle, hangle;
  MatrixD overy( rotationMatrix3D(1,hangle=atan2(-cur_torch.x(),-cur_torch.z())) );
  Vector3D unrot1 = line2vec( cur_torch.lineMatrix() * overy );
  vangle=atan2( -unrot1.y(), -unrot1.z() );
  //std::cout << "Angles: v: " << rad2deg(vangle) << " h: " << rad2deg(hangle) << " angle: " << rad2deg(angle) << "\n";
  torch_.push( Vector2D(hangle,vangle) );

  //std::cout << "Angle: " << rad2deg(angle) << " tip_init_: " << tip_init_ << "\n";

  MatrixD currot = overy * rotationMatrix3D(0,vangle);
  Vector3D tipnow = column2vec( currot * Vector3D(0,0,-270*40.).columnMatrix() ) +
                    column2vec( currot * Vector3D(0,76*40.,0).columnMatrix() ),
           offset = tip_init_ - tipnow;

  //std::cout << "Angle: " << rad2deg(angle) << " tip_init_: " << tip_init_ << " tipnow: " << tipnow << " offset: " << offset << "\n";

  Vector3D final = pv.front() + offset, tip = final+tipnow;
  circ << tip.x() << " " << tip.y() << " " << tip.z() << "\n";
  //std::cout << pv.front() << " <- current  should be -> " << final << "\n";
  //return pv.front();
  return final;
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
