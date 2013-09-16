#ifndef _GRANITE_SPI_INTERFACE_H_
#define _GRANITE_SPI_INTERFACE_H_

#include <stdint.h>
#include <string>

class GraniteSPI {
public:
  uint64_t graniteSetParam( const char *paramname, uint32_t value );
  uint64_t graniteAbsTarget( uint32_t param );
  uint32_t startHoming();
  uint32_t clearFaults();
  uint32_t nope();
  uint32_t getParam( uint8_t i );
  static uint8_t  calcCrc8( uint8_t cmd, uint16_t data );
  static uint32_t graniteDriveCmd( uint16_t cmd, uint16_t data );

private:
  int parameterIndex( std::string paramname );
};

#endif