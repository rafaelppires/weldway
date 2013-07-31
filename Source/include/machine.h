#ifndef _MACHINE_H_
#define _MACHINE_H_

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#define X_AXIS   0x01
#define Y_AXIS   0x02
#define Z_AXIS   0x04
#define A_AXIS   0x08
#define B_AXIS   0x10
#define ALL_AXIS 0x1F

class Machine {
public:

  void axisFindZero( uint8_t axis = ALL_AXIS ) { 
    if( !comm_ ) return;
    comm_->sendCommand( axis, spi.findZero() ); 
  }
  
  void axisMoveTo( long double pos, long double spd, uint8_t axis ) {
    if( !comm_ ) return;
    comm_->sendCommand( axis, spi.speed( spd ) );
    comm_->sendCommand( axis, spi.position( pos ) );
  }

private:
  GraniteSPICmmd spi;
  shared_ptr<AbstractCommunication> comm_;
};

#endif
