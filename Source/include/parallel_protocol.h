#ifndef _PARALLEL_PROTOCOL_H_
#define _PARALLEL_PROTOCOL_H_

#include <master_communicator.h>
#include <parallel_port.h>
#include <granite_spi_interface.h>

#define SPIWRITE_CLK_PIN  2       // SPI Write/Read clock pin

#define SPIREAD_CLK_PIN   17      // SPI Read clock pin
#define SPIREAD_HOME_MSK  (1<<13) // SPI Read home pin mask
#define SPIREAD_DATA_MSK  (1<<15) // SPI Read data pin mask

class ParallelProtocol : public AbstractProtocol {
public:
  ParallelProtocol( uint16_t addr );
  virtual void startHoming( uint8_t );
  virtual void moveTo();
  virtual void executeTrajectory();
  virtual void finish();

private:
  bool findReadHome();
  uint16_t sendWord( uint16_t w, uint32_t pins );
  uint32_t axisToPins( uint8_t axis );
  void delay( uint16_t ns );

  ParallelPort port_;
  GraniteSPI spi_;
};

#endif
