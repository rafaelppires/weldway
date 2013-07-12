#include <simplemotion.h>
#include <string>

class SimpleMotion {
public:
  SimpleMotion( std::string name );
  void startHoming();
  void setMaxSpeed( double value ); // In RPM
  void gotoAbsPosition( double value ); // In Pulses
private:
  std::string name_;
};
