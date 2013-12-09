#include <switch_back.h>

//-----------------------------------------------------------------------------
SwitchBackTrajectory::SwitchBackTrajectory( int32_t fwl, int32_t weldspd ) :
                    fwlen_(fwl), finished_(false), step_(0), torch_on_(false) {
  fwspd_ = 4 * weldspd,
  bwlen_ = fwlen_/2,
  bwspd_ = 2 * weldspd;
  fwinterval_ = 1000. * (fwlen_/TO_PULSES) / (fwspd_/TO_RPM);
  bwinterval_ = 1000. * (bwlen_/TO_PULSES) / (bwspd_/TO_RPM);

  double ax = 4000 / (0.025 * TO_RPM);
  fwspd_ = adjustedSpeed( fwspd_/TO_RPM, ax, fwinterval_/1000. ) * TO_RPM;
  bwspd_ = adjustedSpeed( bwspd_/TO_RPM, ax, bwinterval_/1000. ) * TO_RPM;
  printf("Initiated switch back with fwlen %d bwlen %d intv fw %f intv bw %f\n", fwlen_, bwlen_, fwinterval_, bwinterval_ );
}

//-----------------------------------------------------------------------------
bool SwitchBackTrajectory::finished() {
  return finished_;
}

//-----------------------------------------------------------------------------
bool SwitchBackTrajectory::torchOn() {
  return torch_on_;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 SwitchBackTrajectory::speed() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  if( !step_ ) {
    ret[ X_AXIS ] = 650; // max speed
  } else if( step_ % 2 == 0 ) { // even
    ret[ X_AXIS ] = bwspd_;
  } else { // odd
    ret[ X_AXIS ] = fwspd_;
  }
  last_spd_ = ret[ X_AXIS ];
  return ret;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 SwitchBackTrajectory::position() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  if( !step_ ) {
    ret[ X_AXIS ] = trajectory_init_.x();
    ret[ Y_AXIS ] = trajectory_init_.y();
    torch_on_ = true;
  } else if( step_ % 2 == 0 ) { // even
    ret[ X_AXIS ] = last_pos_ - bwlen_;
  } else { // odd
    ret[ X_AXIS ] = last_pos_ + fwlen_;
  }
  if( ret[ X_AXIS] > trajectory_final_.x() ) {
    ret[ X_AXIS ] = trajectory_final_.x();
    finished_ = true;
  }
  last_pos_ = ret[X_AXIS];
  return ret;
}

//-----------------------------------------------------------------------------
boost::chrono::milliseconds SwitchBackTrajectory::interval() {
  int ret;
  if( !step_ )
    ret = 500 + 1000. * (fabs(current_pos_-trajectory_init_.x())/TO_PULSES) / (650/TO_RPM);
  else if( step_ % 2 == 0 )
    ret = bwinterval_;
  else
    ret = fwinterval_;
  ++step_;
  printf("time %d\n", ret );
  return boost::chrono::milliseconds(ret);
}

//-----------------------------------------------------------------------------
