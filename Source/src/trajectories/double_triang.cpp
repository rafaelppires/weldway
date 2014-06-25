#include <double_triang.h>

//-----------------------------------------------------------------------------
DoubleTriangularTraj::DoubleTriangularTraj( double spd, double lmbd, double ampl,
                                            const Vector3D &rotate_vec, double deg_xang ) : AbstractTrajectory(rotate_vec, deg_xang) {
  double period = (lmbd*TO_RPM) / (spd*TO_PULSES),
         torchspd = (sqrt(ampl*ampl+pow(lmbd/2,2))*4+lmbd)/(period*TO_PULSES); // mm/s

  addRepeatable( 0.5 + rotate_vec.length()/lmbd, lmbd, ampl, torchspd );
  rotate();
}

//-----------------------------------------------------------------------------
void DoubleTriangularTraj::applyCorrection(double spd, double lmbd, double ampl) {

}

//-----------------------------------------------------------------------------
void DoubleTriangularTraj::addRepeatable(int count, double lmbd, double ampl, double spd) {
  int sig = 1;
  for( int i = 0; i < count; ++i, sig *= -1) {
    addR( Vector3D(lmbd/4.,sig*ampl/2.,0), spd );
    addR( Vector3D(lmbd/2.,-sig*ampl,0), spd );
    addR( Vector3D(lmbd/4.,sig*ampl/2.,0), spd );
    addR( Vector3D(-lmbd,0,0), spd );
    addR( Vector3D(lmbd/4.,-sig*ampl/2.,0), spd );
    addR( Vector3D(lmbd/2.,sig*ampl,0), spd );
    addR( Vector3D(lmbd/4.,-sig*ampl/2.,0), spd );
  }
}

//-----------------------------------------------------------------------------
void DoubleTriangularTraj::draft(PositionVector &out, double spd, double l, double ampl) {
  DoubleTriangularTraj e( spd, l, ampl,Vector3D(4*l,0,0), 0 );
  out.clear();
  out.push_back( e.initialOffset() );
  out.insert( out.end(), e.positions_.begin(), e.positions_.end() );
}

//-----------------------------------------------------------------------------
