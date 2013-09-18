#ifndef _PARALLEL_PROTOCOL_H_
#define _PARALLEL_PROTOCOL_H_

#include <master_communicator.h>
#include <parallel_port.h>
#include <granite_spi_interface.h>

#define MANIP_EMERG_PIN   12
#define MANIP_ENABL_PIN   16
#define SPIWRITE_CLK_PIN  2       // SPI Write/Read clock pin
#define SPIWRITE_CLK_MSK  0x4     // SPI clock mask

#define SPIREAD_CLK_PIN   17      // SPI Read clock pin
#define SPIREAD_HOME_MSK  0x2000  // SPI Read home pin mask

#define SPIREAD_DATA_PIN  15      // SPI Read data pin
#define SPIREAD_DATA_MSK  0x8000  // SPI Read data pin mask

struct RetRawCmmd {
  uint8_t  stat;
  uint16_t data;
  uint8_t  crc_match;
};

typedef std::map< uint8_t, RetRawCmmd > RetAxis;

class ParallelProtocol : public AbstractProtocol {
public:
  ParallelProtocol( uint16_t addr );
  virtual void startHoming( uint8_t );
  virtual void moveTo();
  virtual void executeTrajectory();
  virtual void finish();
  virtual void setMaxSpeed( uint16_t spd, uint8_t axis );
  virtual void sendPosCmmds( ConcurrentCmmd & );
  virtual void sendSpdCmmds( ConcurrentCmmd & );

private:
  RetAxis sendRawCommand64( ConcurrentCmmd64 cmmds );
  RetAxis sendRawCommand64( uint64_t cmmd, uint32_t pins );
  ConcurrentCmmd setParam( GraniteParams gp, uint32_t value, uint32_t pins );
  uint8_t axisMask( const ConcurrentCmmd &cmmds );
  RetAxis sendRawCommand( uint32_t cmd, uint32_t pins );
  ConcurrentCmmd getParam(GraniteParams , uint32_t pins);
  bool findReadHome();
  void emergencyCallback( bool );
  ConcurrentCmmd sendWord(uint16_t w[], uint32_t pins );
  ConcurrentCmmd sendWord(uint16_t w, uint32_t pins );
  uint32_t axisToPins( uint8_t axis );
  void delay( uint16_t ns );

  ParallelPort port_;
  GraniteSPI spi_;
};

#endif
