
#include<master_communicator.h>

class KeyPressManager {
public:
  KeyPressManager( MasterCommunicator &mc ): comm_(mc), linear_increment_(1.5), angular_increment_(3.0), speed_(1) {}

  void angularUp();
  void angularDown();
  void angularLeft();
  void angularRight();
  void xaxisLeft();
  void xaxisRight();
  void yaxisFront();
  void yaxisBack();
  void zaxisUp();
  void zaxisDown();

private:
  MasterCommunicator &comm_;
  double speed_, linear_increment_, angular_increment_;
};
