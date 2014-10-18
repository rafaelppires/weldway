#include <double8.h>
#include <e_trajectory.h>
#include <linear_transform.h>
//-----------------------------------------------------------------------------
Double8Trajectory::Double8Trajectory( double spd, double lambda, double ampl,
                                      double rho, TrajectoryTransformPtr tt ) :
  AbstractTrajectory(tt) {
  double total_length = tt->length();
  int period_count = 0.5 + total_length/lambda;
 
  ETrajectory e( spd, lambda/2., ampl/2., rho, tt );
  e.getSingle(psingle_, ssingle_);
 
  addRepeatable( period_count );
  rotate();
}

//-----------------------------------------------------------------------------
void Double8Trajectory::applyCorrection( double spd, double lambda,
                                         double ampl, double rho ) {
  boost::lock_guard<boost::mutex> lock(data_mutex_);

}

//-----------------------------------------------------------------------------
void Double8Trajectory::addRepeatable( uint32_t period_count ) {
  for( uint32_t i = 0; i < period_count; ++i ) {
    size_t sz = psingle_.size();
    for( size_t j = 0; j < sz; ++j ) {
      addR( psingle_[j], ssingle_[j] );
    }
    setReference();
    for( size_t j = 0; j < sz; ++j ) {
      addR( Vector3D(psingle_[j].x(),-psingle_[j].y(),0), ssingle_[j] );
    }
  }
}
//-----------------------------------------------------------------------------
void Double8Trajectory::draft( PositionVector &out, double spd, double l, double ampl, double rho ) {
  Double8Trajectory e( spd, l, ampl, rho, TrajectoryTransformPtr(new LinearTransform(Vector3D(4*l,0,0), 0)));
  out.clear();
  out.push_back( e.initialOffset() );
  out.insert( out.end(), e.positions_.begin(), e.positions_.end() );
}

//-----------------------------------------------------------------------------
