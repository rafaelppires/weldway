#ifndef _GRANITE_SPI_INTERFACE_H_
#define _GRANITE_SPI_INTERFACE_H_

#include <stdint.h>
#include <string>

#define CONTROLMODE_POSITON   1
#define CONTROLMODE_VELOCITY  2
#define CONTROLMODE_TORQUE    3

#define SIMPLESTATUS_BUSY     0
#define SIMPLESTATUS_FAULT    1
#define SIMPLESTATUS_IDLE     2

enum GraniteParams {
  VelocityLimit,
  AccelerationLimit,
  ControlMode,
  CurrentLimitCont,
  CurrentLimitPeak,
  WatchdogTimeout,
  FaultBits,
  StatusBits,
  ReturnDataPayloadType,
  SimpleStatus,
  FollowingError,
  ActualTorque,
  ReturnDataPayload
};

class GraniteSPI {
public:
  uint64_t graniteSetParam( const char *paramname, uint32_t value );
  uint64_t graniteSetParam( GraniteParams p, uint32_t value );
  uint64_t graniteAbsTarget(int32_t param );
  uint32_t startHoming();
  uint32_t clearFaults();
  uint32_t nope();
  uint32_t getParam( GraniteParams i );
  static uint8_t  calcCrc8( uint8_t cmd, uint16_t data );
  static uint32_t graniteDriveCmd( uint16_t cmd, uint16_t data );

private:
  uint64_t setParamCmmd( uint8_t i, uint32_t value );
  int parameterIndex( std::string paramname );
};

#endif
