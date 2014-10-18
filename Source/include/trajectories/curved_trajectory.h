#ifndef CURVEDTRAJECTORY_H
#define CURVEDTRAJECTORY_H
#include <trajectory.h>

//-----------------------------------------------------------------------------
class CurveTrajectory {
public:
  virtual Vector3D f( double t ) = 0;
  virtual double invarclen( double s ) = 0; // inverse of arclen at s (returns t)
  virtual double darclen( double t ) = 0; // derivative of arclength at t
  virtual double arclen( double t ) = 0; // arclen at t
  virtual void getBase( double t, Vector3D &tangent, Vector3D &normal, Vector3D &binormal ) = 0;

  static PositionVector process( const PositionVector &v, CurveTrajectory &c, double t0, int sig ) {
    PositionVector ret;
    PositionVector::const_iterator it = v.begin(), end = v.end();
    double t = t0, alen = c.arclen(t);
    Vector3D initial = -c.f(t);
    for(; it!=end; ++it) {
      t = c.invarclen(alen + sig * it->x());
      Vector3D tan, nor, bin;
      c.getBase(t,tan,nor,bin);
      ret.push_back( initial + c.f(t) + nor * it->y() + bin * it->z() );
    }
    return ret;
  }
};

//-----------------------------------------------------------------------------
class Parabolical : public CurveTrajectory {
public:
  Parabolical( double a ) : a_(a) {}

  double invarclen( double s ) { // inverse of arclength (Newton-Raphson)
    double t = 0, err = 1000;
    while( fabs(err) > 1e-5 ) {
      err = arclen(t) - s;
      t -= err/darclen(t);

    }
    return t;
  }

  Vector3D f( double t ) {
    return Vector3D( t, a_ * t * t, 0 );
  }

  double darclen( double t ) { // derivative of arclength
    return sqrt(1+4*a_*a_*t*t);
  }

  double arclen( double t ) {
    double dsdt = darclen(t);
    return t*dsdt/2+log(2*a_*t+dsdt)/(4*a_);
  }

  void getBase( double t, Vector3D &tangent, Vector3D &normal, Vector3D &binormal ) {
    double dsdt = darclen(t);
    tangent = Vector3D(1/dsdt, 2*a_*t/dsdt, 0);
    normal = Vector3D(-2*a_*t/dsdt, 1/dsdt, 0);
    binormal = tangent.cross(normal);
  }

private:
  double a_;
};

//-----------------------------------------------------------------------------
class Circular : public CurveTrajectory {
public:
  Circular( double r ) : r_(r) {}

  virtual Vector3D f( double t ) {
    return Vector3D(cos(t), sin(t), 0 ) * r_;
  }

  virtual double invarclen( double s ) { // inverse of arclen at s (returns t)
    return s/r_;
  }

  virtual double darclen( double t ) { // derivative of arclength at t
    return r_;
  }

  virtual double arclen( double t ) { // arclen at t
   return r_*t;
  }

  virtual void getBase( double t, Vector3D &tangent, Vector3D &normal, Vector3D &binormal ) {
    double s = sin(t), c = cos(t);
    tangent  = Vector3D(-s, c,0);
    normal   = Vector3D(-c,-s,0);
    binormal = tangent.cross(normal);
  }

private:
  double r_, t0_;
};

//-----------------------------------------------------------------------------
#endif // CURVEDTRAJECTORY_H
