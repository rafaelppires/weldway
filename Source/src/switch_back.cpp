#include <switch_back.h>

//-----------------------------------------------------------------------------
SwitchBackTrajectory::SwitchBackTrajectory( int32_t fwl, int32_t fws,
                                            int32_t bwl, int32_t bws,
                                            PositionConv pc, SpeedConv sc  ) :
    fwlen_(fwl), fwspd_(fws), bwlen_(bwl), bwspd_(bws), finished_(false), step_(0) {
  double spdconv = sc.getConv("rpm"),
         posconv = pc.getConv("pulsos");
  fwinterval_ = 1000. * (fwlen_/posconv) / (fwspd_/spdconv);
  bwinterval_ = 1000. * (bwlen_/posconv) / (bwspd_/spdconv);
  printf("Initiated switch back with fwlen %d bwlen %d intv fw %f intv bw %f\n", fwlen_, bwlen_, fwinterval_, bwinterval_ );
}

//-----------------------------------------------------------------------------
bool SwitchBackTrajectory::finished() {
  return finished_;
}

//-----------------------------------------------------------------------------
uint16_t SwitchBackTrajectory::speed() {
  AbstractProtocol::ConcurrentCmmd ret;
  if( !step_ ) {
    ret[ X_AXIS ] = 650; // max speed
  } else if( step_ % 2 == 0 ) { // even
    ret[ X_AXIS ] = bwspd_;
  } else { // odd
    ret[ X_AXIS ] = fwspd_;
  }
  return ret[ X_AXIS ];
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd SwitchBackTrajectory::position() {
  AbstractProtocol::ConcurrentCmmd ret;
  ret[Y_AXIS] = ret[Z_AXIS] = 0;
  if( !step_ ) {
    ret[ X_AXIS ] = 4000;
  } else if( step_ % 2 == 0 ) { // even
    ret[ X_AXIS ] = last_ - bwlen_;
  } else { // odd
    ret[ X_AXIS ] = last_ + fwlen_;
  }
  if( ret[X_AXIS] > 30000 ) {
    ret[ X_AXIS ] = 30000;
    finished_ = true;
  }
  last_ = ret[X_AXIS];
  return ret;
}

//-----------------------------------------------------------------------------
boost::chrono::milliseconds SwitchBackTrajectory::interval() {
  int ret;
  if( !step_ )
    ret = 3000;
  else if( step_ % 2 == 0 )
    ret = bwinterval_;
  else
    ret = fwinterval_;
  ++step_;
  printf("time %d\n", ret );
  return boost::chrono::milliseconds(ret);
}

//-----------------------------------------------------------------------------
