#ifndef _DEBUG_PROTOCOL_H_
#define _DEBUG_PROTOCOL_H_

#include <master_communicator.h>

class DebugProtocol : public AbstractProtocol {
public:
  
  DebugProtocol();
  virtual void startHoming( uint8_t );
  virtual void startHomingSequence( std::string );
  virtual void moveTo();
  virtual void executeTrajectory();
  virtual void finish();
  virtual void setMaxSpeed( uint16_t spd, uint8_t axis );
  virtual void sendPosCmmds( const ConcurrentCmmd32 & );
  virtual void sendSpdCmmds( const ConcurrentCmmd32 & );
  virtual int32_t getStatus( GraniteParams param, uint8_t axis );
  virtual Vector3I getLastSentPos();
  virtual void startTorch();
  virtual void stopTorch();
  virtual void sendAngularIncrement( AngularDirection dir, double spd, double inc );
  virtual void sendLinearIncrement(uint8_t axis, int32_t spd, int32_t inc );

private:

};

#endif
