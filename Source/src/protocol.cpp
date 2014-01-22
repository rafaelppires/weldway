#include <protocol.h>

//-----------------------------------------------------------------------------
Vector3I AbstractProtocol::getLastSentPos() {
  return Vector3I( lastcmmd_pos_[X_AXIS], lastcmmd_pos_[Y_AXIS], lastcmmd_pos_[Z_AXIS] );
}

//-----------------------------------------------------------------------------
void AbstractProtocol::sendAngularIncrement( AngularDirection dir, double spd, double inc ) {
  if( !homing_done_ ) return;

  ConcurrentCmmd32 speeds, pos;
  while( inc >  360 ) inc -= 360;
  while( inc < -360 ) inc += 360;
  int32_t pulses = 36*400 * inc/360,
          apos = lastcmmd_pos_[A_AXIS],
          bpos = lastcmmd_pos_[B_AXIS];
  speeds[A_AXIS] = speeds[B_AXIS] = spd;
  if( dir == ANGULAR_VERTICAL ) {
    pos[A_AXIS] = apos + pulses;
    pos[B_AXIS] = apos - pulses;
  } else if( dir == ANGULAR_HORIZONTAL ) {
    pos[A_AXIS] = apos + pulses;
    pos[B_AXIS] = apos + pulses;
  }

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
void AbstractProtocol::homingDone() {
 lastcmmd_pos_.clear();
 homing_done_ = true;
}

//-----------------------------------------------------------------------------
