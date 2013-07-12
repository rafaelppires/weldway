#include <simplemotion.h>
#include <simple_motion_interface.h>

//-----------------------------------------------------------------------------
SimpleMotion::SimpleMotion( std::string name ) : name_(name) {

}

//-----------------------------------------------------------------------------
void SimpleMotion::startHoming() {
  /*SM_STATUS stat;
  stat =*/ smCommand( name_.c_str(), "HOMING", 1 );
  //handleStatus(stat);
}

//-----------------------------------------------------------------------------
void SimpleMotion::setMaxSpeed( double value ) {
  /*SM_STATUS stat;
  stat=*/ smSetParam( name_.c_str(), "VelocityLimit", smint32(value) );
  //handleStatus(stat);
}

//-----------------------------------------------------------------------------
void SimpleMotion::gotoAbsPosition( double value ) {
  /*SM_STATUS stat;
  stat=*/smCommand( name_.c_str(), "AbsTarget", smint32(value) );
  //handleStatus(stat);
}
