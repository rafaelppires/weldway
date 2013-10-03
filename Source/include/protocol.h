#ifndef _ABSTRACTPROTOCOL_H_
#define _ABSTRACTPROTOCOL_H_

#include <map>
#include <stdint.h>

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
  typedef std::map<uint8_t, int16_t> ConcurrentCmmd;   // Tuples <Axis,Cmmd> which are sent to be executed concurrently (usually position and speed)
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

#endif