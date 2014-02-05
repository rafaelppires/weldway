#include <keypress_manager.h>

//-----------------------------------------------------------------------------
void KeyPressManager::angularUp() {
  comm_.sendAngularIncrement( ANGULAR_VERTICAL, speed_, angular_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::angularDown() {
  comm_.sendAngularIncrement( ANGULAR_VERTICAL, speed_, -angular_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::angularLeft() {
  comm_.sendAngularIncrement( ANGULAR_HORIZONTAL, speed_, angular_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::angularRight() {
  comm_.sendAngularIncrement( ANGULAR_HORIZONTAL, speed_, -angular_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::xaxisLeft() { // When looking the manipulator from behind
  comm_.sendLinearIncrement( X_AXIS, speed_, -linear_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::xaxisRight() {
  comm_.sendLinearIncrement( X_AXIS, speed_, linear_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::yaxisFront() {
  int32_t incr = -linear_increment_;
  if( comm_.busy() )
    comm_.sendLinearCorrection( Y_AXIS, incr );
  else
    comm_.sendLinearIncrement( Y_AXIS, speed_, incr );
}

//-----------------------------------------------------------------------------
void KeyPressManager::yaxisBack() {
  if( comm_.busy() )
    comm_.sendLinearCorrection( Y_AXIS, linear_increment_ );
  else
    comm_.sendLinearIncrement( Y_AXIS, speed_, linear_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::zaxisUp() {
  if( comm_.busy() )
    comm_.sendLinearCorrection( Z_AXIS, linear_increment_ );
  else
    comm_.sendLinearIncrement( Z_AXIS, speed_, linear_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::zaxisDown() {
  int32_t incr = -linear_increment_;
  if( comm_.busy() )
    comm_.sendLinearCorrection( Z_AXIS, incr );
  else
    comm_.sendLinearIncrement( Z_AXIS, speed_, incr );
}

//-----------------------------------------------------------------------------
