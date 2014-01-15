
#include<master_communicator.h>

class KeyPressManager {
public:
  KeyPressManager( MasterCommunicator &mc ): comm_(mc), linear_increment_(40) /* 1 mm */, angular_increment_(90), speed_(200) {}

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
  int32_t speed_, linear_increment_, angular_increment_;
};
