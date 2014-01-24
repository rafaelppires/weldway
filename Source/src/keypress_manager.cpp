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
  comm_.sendLinearIncrement( Y_AXIS, speed_, -linear_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::yaxisBack() {
  comm_.sendLinearIncrement( Y_AXIS, speed_, linear_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::zaxisUp() {
  comm_.sendLinearIncrement( Z_AXIS, speed_, linear_increment_ );
}

//-----------------------------------------------------------------------------
void KeyPressManager::zaxisDown() {
  comm_.sendLinearIncrement( Z_AXIS, speed_, -linear_increment_ );
}

//-----------------------------------------------------------------------------
