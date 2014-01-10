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
  comm_.sendIncrement( X_AXIS, speed_, -linear_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::xaxisRight() {
  comm_.sendIncrement( X_AXIS, speed_, linear_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::yaxisFront() {
  comm_.sendIncrement( Y_AXIS, speed_, -linear_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::yaxisBack() {
  comm_.sendIncrement( Y_AXIS, speed_, linear_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::zaxisUp() {
  comm_.sendIncrement( Z_AXIS, speed_, linear_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::zaxisDown() {
  comm_.sendIncrement( Z_AXIS, speed_, -linear_increment_ );
}

//-----------------------------------------------------------------------------
