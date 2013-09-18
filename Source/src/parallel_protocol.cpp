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
  for( int i = 0; i < 5; ++i ) {
    port_.invertPinSync( SPIREAD_CLK_PIN );
    if( !port_.readPins( SPIREAD_HOME_MSK ) )
      return true;

    port_.invertPinSync( SPIREAD_CLK_PIN );
    if( !port_.readPins( SPIREAD_HOME_MSK ) )
      return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
// Sends 16 bits concurrently to all the commanded pins (bitmask)
//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd ParallelProtocol::sendWord( uint16_t w, uint32_t pins ) {
  uint16_t cmmds[ AXIS_CNT ];
  for( int i = 0; i < AXIS_CNT; ++i )
    cmmds[i] = w;
  return sendWord( cmmds, pins );
}

//-----------------------------------------------------------------------------
// Sends concurrently 16bit commands whithin "w" vector
//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd ParallelProtocol::sendWord( uint16_t w[AXIS_CNT], uint32_t pins ) {
  ConcurrentCmmd ret;
  printf("Sent: %X on pins %X ", w, pins);
  //port_.startLogging();
  uint32_t word = 0;
  port_.setLowPinSync( SPIWRITE_CLK_PIN );
  for( int i = 0; i < 16; ++i ) {

    for( int j = 0; j < AXIS_CNT; ++j ) {
      uint32_t bit = 1 << (3 + j); // pins 3 to 9 are SPI MOSI
      if( (w[j] >> i) & 1 )
        word |= bit; // set
      else
        word &= ~bit; // reset
    }

    // data is set on CLK rising edge
    port_.writePinsSync( word | SPIWRITE_CLK_MSK, pins | SPIWRITE_CLK_MSK );
    delay( 5000 ); // should be 6us, but when added to other delays it reaches about 20 us
    // the two samples (read/write) on the CLK falling edge
    port_.setLowPinSync( SPIWRITE_CLK_PIN );
    // Read reply here
#if SINGLEDRIVE_CONNECT
    uint8_t bit = ~(port_.readPins( SPIREAD_DATA_MSK ) >> SPIREAD_DATA_PIN) & 1;
    if( ret.find(X_AXIS) == ret.end() ) ret[X_AXIS] = 0;
    ret[X_AXIS]  |= bit << i;
#else // Control Board
#endif
    delay( 5000 ); // should be 6us, but when added to other delays it reaches about 20 us
  }
  port_.writePinsSync( 0, pins );
  //port_.stopLogging();
  return ret;
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
  return ret & 0x3F8; // Pins 3 to 9
}

//-----------------------------------------------------------------------------
uint8_t ParallelProtocol::axisMask( const ConcurrentCmmd &cmmds ) {
  uint8_t ret;
  ConcurrentCmmd::const_iterator it = cmmds.begin(), end = cmmds.end();
  for(; it != end; ++it )
    ret |= it->first;
  return ret;
}

//-----------------------------------------------------------------------------
RetAxis ParallelProtocol::sendRawCommand32( uint32_t cmd, uint32_t pins ) {
  ConcurrentCmmd32 cmmds;
  for( int i = 0; i < AXIS_CNT; ++i )
    if( pins & (1<<(3+i)) )
      cmmds[1<<i] = cmd;
  return sendRawCommand32(cmmds);
}

//-----------------------------------------------------------------------------
RetAxis ParallelProtocol::sendRawCommand32( ConcurrentCmmd32 &cmmds ) {
  RetAxis ret;
  uint16_t w1[AXIS_CNT], w2[AXIS_CNT];
  uint32_t pins = 0;
  for( int i = 0; i < AXIS_CNT; ++i ) {
    uint8_t axis = 1<<i;
    if( cmmds.find( axis ) != cmmds.end() ) {
      w1[i] = cmmds[axis] >> 16;
      w2[i] = cmmds[axis] & 0xFFFF;
      pins |= 1<<(3+i);
    } else
      w1[i] = w2[i] = 0;
  }

  ConcurrentCmmd ret1, ret2;
  ret1 = sendWord( w1, pins );
  ret2 = sendWord( w2, pins );

  ConcurrentCmmd::iterator it = ret1.begin(), end = ret1.end();
  for(; it != end; ++it) {
    uint8_t axis = it->first;
    uint32_t r = ret2[axis] << 16 | ret1[axis];
    ret[ axis ].stat = r >> 24;
    ret[ axis ].data = (r >> 8) & 0xFFFF;
    ret[ axis ].crc_match  = GraniteSPI::calcCrc8(ret[axis].stat,ret[axis].data) == (r & 0xFF);
  }

  return ret;
}

//-----------------------------------------------------------------------------
void ParallelProtocol::startHoming( uint8_t axis ) {
  sendRawCommand( spi_.startHoming(), axisToPins( axis ) );
}

//-----------------------------------------------------------------------------
void ParallelProtocol::setMaxSpeed( uint16_t spd, uint8_t axis ) {
  ConcurrentCmmd cmmds;
  for( uint32_t i = 1; i < AXIS_ALL; i <<= 1 )
    if( axis & i ) cmmds[i] = spd;
  sendSpdCmmds( cmmds );
}

//-----------------------------------------------------------------------------
void ParallelProtocol::sendPosCmmds( ConcurrentCmmd &cmmds ) {
  uint32_t pins = axisToPins( axisMask( cmmds ) );
  ConcurrentCmmd ret = getParam( ControlMode, pins );
  setParam( ControlMode, CONTROLMODE_POSITON, pins ); // position mode

  ConcurrentCmmd64 pos_cmmds;
  ConcurrentCmmd::iterator it = cmmds.begin(), end = cmmds.end();
  for(; it != end; ++it)
    pos_cmmds[ it->first ] = spi_.graniteAbsTarget( it->second );
  sendRawCommand64( pos_cmmds );
}

//-----------------------------------------------------------------------------
// Sends 64 bit commands to a number of drivers concurrently
//-----------------------------------------------------------------------------
RetAxis ParallelProtocol::sendRawCommand64( ConcurrentCmmd64 cmmds ) {
  ConcurrentCmmd32 cmmds1, cmmds2;
  ConcurrentCmmd64::iterator it = cmmds.begin(), end = cmmds.end();
  for(; it != end; ++it) {
    cmmds1[ it->first ] = it->second >> 32;
    cmmds2[ it->first ] = it->second & 0xFFFFFFFF;
  }

  sendRawCommand32( cmmds1 );
  return sendRawCommand32( cmmds2 );
}

//-----------------------------------------------------------------------------
// Sends one single 64 bit command to a number of drivers concurrently
//-----------------------------------------------------------------------------
RetAxis ParallelProtocol::sendRawCommand64( uint64_t cmmd, uint32_t pins ) {
  sendRawCommand32( cmmd >> 32, pins );
  return sendRawCommand32( cmmd & 0xFFFFFFFF, pins );
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd ParallelProtocol::setParam( GraniteParams gp, uint32_t value, uint32_t pins ) {
  ConcurrentCmmd ret;
  sendRawCommand( spi_.getParam(gp), pins );
  sendRawCommand( spi_.nope(), pins );

  RetAxis rraw = sendRawCommand64( spi_.graniteSetParam(gp, value), pins ); // this acttually returns the value
  RetAxis::iterator it = rraw.begin(), end = rraw.end();
  for(; it != end; ++it )
    if( it->second.crc_match )
      ret[ it->first ] = it->second.data;
    else
      std::cerr << "CRC on axis " << int(it->first) << " did not match\n";

  return ret;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd ParallelProtocol::getParam( GraniteParams gp, uint32_t pins ) {
  ConcurrentCmmd ret;

  sendRawCommand( spi_.getParam(gp), pins );
  sendRawCommand( spi_.nope(), pins );

  RetAxis rraw = sendRawCommand( spi_.nope(), pins ); // this acttually returns the value
  RetAxis::iterator it = rraw.begin(), end = rraw.end();
  for(; it != end; ++it )
    if( it->second.crc_match )
      ret[ it->first ] = it->second.data;
    else
      std::cerr << "CRC on axis " << int(it->first) << " did not match\n";

  return ret;
}

//-----------------------------------------------------------------------------
void ParallelProtocol::sendSpdCmmds( ConcurrentCmmd &cmmds ) {

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
