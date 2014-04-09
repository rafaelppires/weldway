#ifndef _DEBUG_PROTOCOL_H_
#define _DEBUG_PROTOCOL_H_

#include <master_communicator.h>
#include <fstream>

class DebugProtocol : public AbstractProtocol {
public:
  
  DebugProtocol();
  ~DebugProtocol();
  virtual void startHoming( uint8_t );
  virtual void startHomingSequence();
  virtual void setMaxSpeed( uint16_t spd, uint8_t axis );
  virtual void sendPosCmmds(const ConcurrentCmmd32 & cmmds);
  virtual void sendSpdCmmds( const ConcurrentCmmd32 & );
  virtual int32_t getStatus( GraniteParams param, uint8_t axis );
  virtual void startTorch();
  virtual void stopTorch();

private:
  std::ofstream posfile_;
  ConcurrentCmmd32 last_pos_;
};

#endif
