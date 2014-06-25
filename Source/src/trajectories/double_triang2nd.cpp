#include <double_triang2nd.h>

//-----------------------------------------------------------------------------
DoubleTriangular2ndTraj::DoubleTriangular2ndTraj( double spd, double lmbd, double ampl,
                                            const Vector3D &rotate_vec, double deg_xang ) : AbstractTrajectory(rotate_vec, deg_xang) {
  double period = (lmbd*TO_RPM) / (spd*TO_PULSES),
         torchspd = (sqrt(ampl*ampl+lmbd*lmbd)*2+lmbd)/(period*TO_PULSES); // mm/s

  addRepeatable( 0.5 + rotate_vec.length()/lmbd, lmbd, ampl, torchspd );
  rotate();
}

//-----------------------------------------------------------------------------
void DoubleTriangular2ndTraj::applyCorrection(double spd, double lmbd, double ampl) {

}

//-----------------------------------------------------------------------------
void DoubleTriangular2ndTraj::addRepeatable(int count, double lmbd, double ampl, double spd) {
  int sig = 1;
  for( int i = 0; i < count; ++i, sig *= -1) {
    addR( Vector3D(lmbd/2.,sig*ampl/2.,0), spd );
    addR( Vector3D(lmbd/2.,-sig*ampl/2.,0), spd );
    addR( Vector3D(-lmbd,0,0), spd );
    addR( Vector3D(lmbd/2.,-sig*ampl/2.,0), spd );
    addR( Vector3D(lmbd/2.,sig*ampl/2.,0), spd );
  }
}

//-----------------------------------------------------------------------------
void DoubleTriangular2ndTraj::draft(PositionVector &out, double spd, double l, double ampl) {
  DoubleTriangular2ndTraj e( spd, l, ampl,Vector3D(4*l,0,0), 0 );
  out.clear();
  out.push_back( e.initialOffset() );
  out.insert( out.end(), e.positions_.begin(), e.positions_.end() );
}

//-----------------------------------------------------------------------------

