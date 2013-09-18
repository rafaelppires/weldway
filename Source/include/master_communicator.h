#ifndef _MASTER_COMMUNICATOR_H_
#define _MASTER_COMMUNICATOR_H_

#include <stdint.h>
#include <map>
#include <boost/shared_ptr.hpp>

#define X_AXIS   0x01
#define Y_AXIS   0x02
#define Z_AXIS   0x04
#define A_AXIS   0x08
#define B_AXIS   0x10
#define AXIS_ALL 0x1F
#define AXIS_CNT 5

//-----------------------------------------------------------------------------
enum CommType {
  PARALLEL,
  USB_HUB,
  USB_PROXY
};

//-----------------------------------------------------------------------------
class AbstractProtocol {
public:
  typedef std::map<uint8_t,uint16_t> ConcurrentCmmd;   // Tuples <Axis,Cmmd> which are sent to be executed concurrently (usually position and speed)
  typedef std::map<uint8_t,uint32_t> ConcurrentCmmd32; // Same as above, with a 32bit word
  typedef std::map<uint8_t,uint64_t> ConcurrentCmmd64; // Same as above, with a 64bit word

  AbstractProtocol( CommType t ) : type_(t) {}
  virtual void startHoming( uint8_t ) {}
  virtual void moveTo() {}
  virtual void executeTrajectory() {}
  virtual void finish() {}  
  virtual void setMaxSpeed( uint16_t, uint8_t ) {}
  virtual void sendPosCmmds( ConcurrentCmmd & ) {}
  virtual void sendSpdCmmds( ConcurrentCmmd & ) {}

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
  bool setMaxSpeed(uint16_t speed_rpm, uint8_t axis);
  bool sendPosCmmds(AbstractProtocol::ConcurrentCmmd & cmmds);
  
private:
  MasterCommunicator() {}
  MasterCommunicator(MasterCommunicator const&);
  void operator=(MasterCommunicator const&);

  boost::shared_ptr< AbstractProtocol > comm_;
};

#endif
