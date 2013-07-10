#include <simplemotion.h>
#include <simple_motion_interface.h>

//-----------------------------------------------------------------------------
SimpleMotion::SimpleMotion( std::string name ) : name_(name) {

}

//-----------------------------------------------------------------------------
void SimpleMotion::startHoming() {
  SM_STATUS stat;
  stat = smCommand( name_.c_str(), "HOMING", 1 );
  //handleStatus(stat);
}
