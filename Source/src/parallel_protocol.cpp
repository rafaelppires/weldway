#include <parallel_protocol.h>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
using boost::chrono::high_resolution_clock;
using boost::chrono::nanoseconds;

//-----------------------------------------------------------------------------
void ParallelProtocol::delay( uint16_t ns ) {
  high_resolution_clock::time_point now, start;
  nanoseconds diff;
  start = high_resolution_clock::now();
  do {
    boost::this_thread::yield();
    now = high_resolution_clock::now();
    diff = now - start;
  } while( diff.count() < ns );
}

//-----------------------------------------------------------------------------
bool ParallelProtocol::findReadHome() {
  for( int i = 0; i < 8; ++i ) {
    port_.setHighPinSync( SPIREAD_CLK_PIN );
    port_.setLowPinSync( SPIREAD_CLK_PIN );
    if( port_.readPins( SPIREAD_DATA_MSK ) )
      return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
// Sends 16 bits concurrently to all the commanded pins (bitmask)
//-----------------------------------------------------------------------------
uint16_t ParallelProtocol::sendWord( uint16_t w, uint32_t pins ) {
  for( int i = 0; i < 16; ++i ) {
    port_.setHighPinSync( SPIWRITE_CLK_PIN );
    port_.writePinsSync( ((w>>i) & 1) ? pins : 0, pins );
    delay( 6000 );
    port_.setLowPinSync( SPIWRITE_CLK_PIN );
    // Read reply here
    delay( 6000 );
  }
  return 0;
}

//-----------------------------------------------------------------------------
ParallelProtocol::ParallelProtocol( uint16_t addr ) :
    AbstractProtocol( PARALLEL ), port_(addr) {
  port_.startSquareSignal( MANIP_ENABL_PIN, 1000. ); // Pin 16 - 1kHz
  port_.startReadingPin( MANIP_EMERG_PIN, 1./3 /*s*/,
                         std::bind1st( std::mem_fun(&ParallelProtocol::emergencyCallback), this ) );
}

//-----------------------------------------------------------------------------
void ParallelProtocol::emergencyCallback( bool st ) {
  std::cout << "emerg now " << int(st) << "\n";
}

//-----------------------------------------------------------------------------
uint32_t ParallelProtocol::axisToPins( uint8_t axis ) {
  uint32_t ret = 0;
  if( axis & X_AXIS ) ret |= 1 << 3;
  if( axis & Y_AXIS ) ret |= 1 << 4;
  if( axis & Z_AXIS ) ret |= 1 << 5;
  if( axis & A_AXIS ) ret |= 1 << 6;
  if( axis & B_AXIS ) ret |= 1 << 7;
  return ret;
}

//-----------------------------------------------------------------------------
void ParallelProtocol::startHoming( uint8_t axis ) {
  uint32_t cmd = spi_.startHoming(),
           pins = axisToPins( axis );
  uint16_t ret1, ret2;

  ret1 = sendWord( cmd >> 16,    pins );
  ret2 = sendWord( cmd & 0xFFFF, pins );
}

//-----------------------------------------------------------------------------
void ParallelProtocol::moveTo() {

}

//-----------------------------------------------------------------------------
void ParallelProtocol::executeTrajectory() {

}

//-----------------------------------------------------------------------------
void ParallelProtocol::finish() {

}

//-----------------------------------------------------------------------------
