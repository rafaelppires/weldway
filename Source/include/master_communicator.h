#ifndef _MASTER_COMMUNICATOR_H_
#define _MASTER_COMMUNICATOR_H_

#include <stdint.h>
#include <boost/shared_ptr.hpp>

#define X_AXIS   0x01
#define Y_AXIS   0x02
#define Z_AXIS   0x04
#define A_AXIS   0x08
#define B_AXIS   0x10
#define AXIS_ALL 0x1F

//-----------------------------------------------------------------------------
enum CommType {
  PARALLEL,
  USB_HUB,
  USB_PROXY
};

//-----------------------------------------------------------------------------
class AbstractProtocol {
public:
  AbstractProtocol( CommType t ) : type_(t) {}
  virtual void startHoming( uint8_t ) {}
  virtual void moveTo() {}
  virtual void executeTrajectory() {}
  virtual void finish() {}
private:
  CommType type_;
};

//-----------------------------------------------------------------------------
class MasterCommunicator {
public:
  // Singleton
  static MasterCommunicator& getInstance() {
    static MasterCommunicator instance;
    return instance;
  }

  void setupParallelPort( uint16_t addr );
  bool startHoming( uint8_t axis );
  
private:
  MasterCommunicator() {}
  MasterCommunicator(MasterCommunicator const&);
  void operator=(MasterCommunicator const&);

  boost::shared_ptr< AbstractProtocol > comm_;
};

#endif
