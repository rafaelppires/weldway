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

#define TORCH_ENABLE_PIN  1

struct RetRawCmmd {
  uint8_t  stat;
  uint16_t data;
  uint8_t  crc_match;
};

typedef boost::function< void() > EmergencyCallbackType;
typedef std::map< uint8_t, RetRawCmmd > RetAxis;

//-----------------------------------------------------------------------------
class ParallelProtocol : public AbstractProtocol {
public:
  ParallelProtocol( uint16_t addr );
  virtual void startHoming( uint8_t );
  virtual void startHomingSequence( std::string );
  virtual void setMaxSpeed( uint16_t spd, uint8_t axis );
  virtual void sendPosCmmds( const ConcurrentCmmd32 & );
  virtual void sendSpdCmmds( const ConcurrentCmmd32 & );
  virtual int32_t getStatus( GraniteParams param, uint8_t axis );
  virtual void startTorch();
  virtual void stopTorch();
  void setEmergencyCallback( EmergencyCallbackType cback );
  virtual void homingDone();

private:
  RetAxis sendRawCommand32( uint32_t cmd, uint32_t pins ); // same command to pins in the mask "pins"
  RetAxis sendRawCommand32( ConcurrentCmmd32 &cmmds );     // different commands

  RetAxis sendRawCommand64( uint64_t cmmd, uint32_t pins );
  RetAxis sendRawCommand64( ConcurrentCmmd64 cmmds );

  ConcurrentCmmd setParam( GraniteParams gp, uint32_t value, uint32_t pins );
  uint8_t axisMask(const ConcurrentCmmd32 &cmmds );
  ConcurrentCmmd getParam(GraniteParams , uint32_t pins);
  bool findReadHome(uint8_t);
  void emergencyCallback( bool );
  ConcurrentCmmd sendWord(uint16_t w[], uint32_t pins );
  ConcurrentCmmd sendWord(uint16_t w, uint32_t pins );
  uint32_t axisToPins( uint8_t axis );
  void delay( uint16_t ns );
  void updatePosition( const ConcurrentCmmd32 &cmmds );

  uint8_t reply_axis_;
  ParallelPort port_;
  GraniteSPI spi_;
  boost::thread *homing_thread_;
  EmergencyCallbackType emergency_callback_;
};

//-----------------------------------------------------------------------------
class HomingSequencer {
public:
  HomingSequencer( std::string, ParallelProtocol &p ) : protocol_(p) {}

  void operator()();
private:
  ParallelProtocol &protocol_;
};

//-----------------------------------------------------------------------------

#endif
