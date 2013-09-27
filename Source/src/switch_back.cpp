#include <switch_back.h>

//-----------------------------------------------------------------------------
SwitchBackTrajectory::SwitchBackTrajectory( int32_t fwl, int32_t fws,
                                            int32_t bwl, int32_t bws,
                                            PositionConv pc, SpeedConv sc  ) :
    fwlen_(fwl), fwspd_(fws), bwlen_(fws), bwspd_(bwl), finished_(false), step_(0) {
  double spdconv = sc.getConv("rpm"),
         posconv = pc.getConv("pulsos");
  fwinterval_ = 1000. * (fwlen_/posconv) / (fwspd_/spdconv);
  bwinterval_ = 1000. * (bwlen_/posconv) / (bwspd_/spdconv);
}

//-----------------------------------------------------------------------------
bool SwitchBackTrajectory::finished() {
  return finished_;
}

//-----------------------------------------------------------------------------
uint16_t SwitchBackTrajectory::speed() {
  AbstractProtocol::ConcurrentCmmd ret;
  if( !step_ ) {
    ret[ X_AXIS ] = 600; // max speed
  } else if( step_ % 2 == 0 ) { // even
    ret[ X_AXIS ] = bwspd_;
  } else { // odd
    ret[ X_AXIS ] = fwspd_;
  }
  printf("spd %d ", ret[X_AXIS]);
  return ret[ X_AXIS ];
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd SwitchBackTrajectory::position() {
  AbstractProtocol::ConcurrentCmmd ret;
  if( !step_ ) {
    ret[ X_AXIS ] = 0;
  } else if( step_ % 2 == 0 ) { // even
    ret[ X_AXIS ] = last_ + bwlen_;
  } else { // odd
    ret[ X_AXIS ] = last_ + fwlen_;
  }
  if( ret[X_AXIS] > 35000 ) {
    ret[ X_AXIS ] = 35000;
    finished_ = true;
  }
  last_ = ret[X_AXIS];
  printf("pos %d ", ret[X_AXIS]);
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
