#include <triangular.h>
#include <units.h>

//-----------------------------------------------------------------------------
TriangularTrajectory::TriangularTrajectory( int32_t spd, double freq, int32_t ampl ) :
                                      weld_spd_(spd), amplitude_(ampl), step_(0) {
  vy_ = 2. * (ampl/TO_PULSES) * freq * TO_RPM;
  interval_ = 1000. / ( 2. * freq );
  total_time_ = 1000. * ((30000-4000)/TO_PULSES) / (spd/TO_RPM);
  printf("Triangular Spd %d Freq %f Ampl %d Vy %d Inter %f Total %f\n",
         spd, freq, ampl, vy_, interval_, total_time_ );
}

//-----------------------------------------------------------------------------
bool TriangularTrajectory::finished() {
  if( interval_ * step_ > total_time_  )
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
  int32_t yoffset = 400; // 10 mm
  if( !step_ ) {
    ret[ X_AXIS ] = 4000;
    ret[ Y_AXIS ] = -( yoffset + amplitude_ / 2 );
  } else if( step_ % 2 == 0 ) { // even
    ret[ Y_AXIS ] = -yoffset;
  } else { // odd
    ret[ Y_AXIS ] = -( yoffset + amplitude_ );
    if( step_ == 1 )
      ret[ X_AXIS ] = 30000;
  }
  return ret;
}

//-----------------------------------------------------------------------------
boost::chrono::milliseconds TriangularTrajectory::interval() {
  int ret;
  if( !step_ )
    ret = 3000;
  else
    ret = interval_;
  ++step_;
  return boost::chrono::milliseconds( ret );
}

//-----------------------------------------------------------------------------
