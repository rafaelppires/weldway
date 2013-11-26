#include <triangular.h>
#include <units.h>
#include <math.h>

//-----------------------------------------------------------------------------
TriangularTrajectory::TriangularTrajectory( int32_t spd, double freq, int32_t ampl,
                                            uint32_t sstop, uint32_t istop ) :
    weld_spd_(spd), amplitude_(ampl), step_(0), sstop_(sstop), istop_(istop) {

  interval_ = 1000. / ( 2. * freq );
  vy_ = 2. * (ampl/TO_PULSES) * freq * TO_RPM;
  double ay = 4000 / (0.05 * TO_RPM);
  vy_ = adjustedSpeed( vy_/TO_RPM, ay, interval_/1000. ) * TO_RPM;

  interval_ = 1000. / ( 2. * freq );
  total_time_ = interval_;
  printf("Triangular Spd %d Freq %f Ampl %d Vy %d Inter %f Total %f\n",
         spd, freq, ampl, vy_, interval_, total_time_ );
}

//-----------------------------------------------------------------------------
bool TriangularTrajectory::finished() {
  if( (interval_ + istop_ + sstop_) * step_ > total_time_  )
    return true;
  return false;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 TriangularTrajectory::speed() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  if( !step_ ) {
    ret[ X_AXIS ] = ret[ Y_AXIS ] = 650; // max speed
  } else if( step_ == 1 ) {
    ret[ X_AXIS ] = weld_spd_;
    ret[ Y_AXIS ] = vy_;
  }
  return ret;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 TriangularTrajectory::position() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  int32_t yoffset = trajectory_init_.y(); // 10 mm
  if( !step_ ) {
    total_time_ = 1000. * (fabs(trajectory_final_ - trajectory_init_.x())/TO_PULSES) / (weld_spd_/TO_RPM);
    ret[ X_AXIS ] = trajectory_init_.x();
    ret[ Y_AXIS ] = -( yoffset + amplitude_ / 2 );
  } else if( step_ % 2 == 0 ) { // even
    ret[ Y_AXIS ] = -yoffset;
  } else { // odd
    ret[ Y_AXIS ] = -( yoffset + amplitude_ );
    if( step_ == 1 )
      ret[ X_AXIS ] = trajectory_final_;
  }
  return ret;
}

//-----------------------------------------------------------------------------
boost::chrono::milliseconds TriangularTrajectory::interval() {
  int ret;
  if( !step_ )
    ret = 500 + 1000. * (fabs(current_pos_-trajectory_init_.x())/TO_PULSES) / (650/TO_RPM);
  else if( step_ % 2 == 0 )
    ret = interval_ + istop_;
  else
    ret = interval_ + sstop_;
  ++step_;
  return boost::chrono::milliseconds( ret );
}

//-----------------------------------------------------------------------------
