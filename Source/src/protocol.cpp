#include <protocol.h>

//-----------------------------------------------------------------------------
Vector3I AbstractProtocol::getLastSentPos() {
  return Vector3I( lastcmmd_pos_[X_AXIS], -lastcmmd_pos_[Y_AXIS], lastcmmd_pos_[Z_AXIS] );
}

//-----------------------------------------------------------------------------
Vector2I AbstractProtocol::getLastSentTorch() {
  return Vector2I( lastcmmd_pos_[A_AXIS], lastcmmd_pos_[B_AXIS] );
}

//-----------------------------------------------------------------------------
Vector2I AbstractProtocol::angularPulsesOffset( AngularDirection dir, double inc ) {
  Vector2I ret;
  while( inc >  360 ) inc -= 360;
  while( inc < -360 ) inc += 360;
  int32_t pulses = 36.*400. * inc/360.;
  if( dir == ANGULAR_VERTICAL ) {
    ret = Vector2I( pulses, -pulses );
  } else if( dir == ANGULAR_HORIZONTAL ) {
    ret = Vector2I( pulses, pulses );
  }
  return ret;
}

//-----------------------------------------------------------------------------
void AbstractProtocol::sendAngularIncrement( AngularDirection dir, double spd, double inc ) {
  if( !homing_done_ ) return;

  ConcurrentCmmd32 speeds, pos;
  Vector2I angpos( getLastSentTorch() );
  angpos += angularPulsesOffset(dir,inc);
  speeds[A_AXIS] = speeds[B_AXIS] = spd;

  pos[A_AXIS] = angpos.x();
  pos[B_AXIS] = angpos.y();

  sendSpdCmmds( speeds );
  sendPosCmmds( pos );
}

//-----------------------------------------------------------------------------
void AbstractProtocol::sendLinearIncrement( uint8_t axis, int32_t spd, int32_t inc ) {
  if( !homing_done_ ) return;

  ConcurrentCmmd32 speeds, pos;
  int32_t cpos = 0;
  if( lastcmmd_pos_.find(axis) != lastcmmd_pos_.end() )
    cpos = lastcmmd_pos_[axis];

  speeds[ axis ] = spd;
  pos[ axis ] = cpos + inc;

  sendSpdCmmds( speeds );
  sendPosCmmds( pos );
}

//-----------------------------------------------------------------------------
void AbstractProtocol::homingFinished() {
 lastcmmd_pos_.clear();
 homing_done_ = true;
}

//-----------------------------------------------------------------------------
