#include <parallel_protocol.h>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <fstream>
#include <units.h>
#include <granite/vsd_cmd.h>
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
bool ParallelProtocol::findReadHome( uint8_t axis ) { // 0 to 7
  for( int i = 0; i < 8; ++i ) {
    port_.invertPinSync( SPIREAD_CLK_PIN );
    delay( 50000 ); // 50 us
    if( !port_.readPins( SPIREAD_HOME_MSK ) )
      break;
  }

  for( uint8_t i = 0; i < axis+1; ++i )
    port_.invertPinSync( SPIREAD_CLK_PIN );

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
  uint32_t word = 0;
  uint8_t axis = 0;
  for( uint8_t i = 0; i < 8; ++i ) {
	axis = 1 << i;
    if( axis == reply_axis_ ) {
      findReadHome( i );
      break;
    }
  }
  ret[axis] = 0;

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
    delay( 5000 ); // should be 6us, but when added to other delays it reaches about 20 us
    // Read reply here
    uint8_t bit = ~(port_.readPins( SPIREAD_DATA_MSK ) >> SPIREAD_DATA_PIN) & 1;
    ret[axis] |= bit << i;
  }
  //printf("ret %X\n", ret[X_AXIS]);
  port_.writePinsSync( 0, pins );
  //port_.stopLogging();
  return ret;
}

//-----------------------------------------------------------------------------
ParallelProtocol::ParallelProtocol( uint16_t addr ) :
    AbstractProtocol( PARALLEL ), port_(addr), homing_thread_(0), homing_done_(false) {
  port_.startSquareSignal( MANIP_ENABL_PIN, 1000. ); // Pin 16 - 1kHz
  port_.startReadingPin( MANIP_EMERG_PIN, 1./3 /*s*/,
                         std::bind1st( std::mem_fun(&ParallelProtocol::emergencyCallback), this ) );
}

//-----------------------------------------------------------------------------
void ParallelProtocol::startTorch() {
  port_.setHighPinSync( TORCH_ENABLE_PIN );
}

//-----------------------------------------------------------------------------
void ParallelProtocol::stopTorch() {
  port_.setLowPinSync( TORCH_ENABLE_PIN );
}

//-----------------------------------------------------------------------------
void ParallelProtocol::emergencyCallback( bool inactive ) {
  std::cout << "emerg now " << int(inactive) << "\n";
  if( inactive ) return;
  homing_done_ = false;
  if( bool(emergency_callback_) )
    emergency_callback_();
}

//-----------------------------------------------------------------------------
void ParallelProtocol::setEmergencyCallback( EmergencyCallbackType cback ) {
  emergency_callback_ = cback;
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
uint8_t ParallelProtocol::axisMask( const ConcurrentCmmd32 &cmmds ) {
  uint8_t ret;
  ConcurrentCmmd32::const_iterator it = cmmds.begin(), end = cmmds.end();
  for(; it != end; ++it )
    ret |= it->first;
  return ret;
}

//-----------------------------------------------------------------------------
// Sends one single 64 bit command to a number of drivers concurrently
//-----------------------------------------------------------------------------
RetAxis ParallelProtocol::sendRawCommand64( uint64_t cmmd, uint32_t pins ) {
  sendRawCommand32( cmmd >> 32, pins );
  return sendRawCommand32( cmmd & 0xFFFFFFFF, pins );
}

//-----------------------------------------------------------------------------
// Sends 64 bit commands to a number of drivers concurrently
//-----------------------------------------------------------------------------
RetAxis ParallelProtocol::sendRawCommand64( ConcurrentCmmd64 cmmds ) {
  ConcurrentCmmd32 cmmds1, cmmds2;
  ConcurrentCmmd64::iterator it = cmmds.begin(), end = cmmds.end();
  for(; it != end; ++it) {
    //printf("<%X,%llX> ", it->first, it->second );
    cmmds1[ it->first ] = it->second >> 32;
    cmmds2[ it->first ] = it->second & 0xFFFFFFFF;
  }
  printf("\n");
  fflush( stdout );

  sendRawCommand32( cmmds1 );
  return sendRawCommand32( cmmds2 );
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
    //if( !ret[ axis ].crc_match ) { fprintf( stderr, "axis %X CRC not match\n", axis ); fflush(stderr); }
  }

  return ret;
}

//-----------------------------------------------------------------------------
void ParallelProtocol::startHoming( uint8_t axis ) {
  ConcurrentCmmd32 cmmd;
  uint32_t nope = spi_.nope();
  for( uint8_t i = 1; i < AXIS_ALL; i <<= 1) {
    if( i & axis ) cmmd[i] = spi_.startHoming();
    else           cmmd[i] = nope;
  }
  sendRawCommand32( cmmd );
}
//-----------------------------------------------------------------------------
class HomingSequencer {
public:
  HomingSequencer( std::string, ParallelProtocol &p ) : protocol_(p) {}

  void operator()() {
    protocol_.startHoming( A_AXIS | X_AXIS | Y_AXIS | Z_AXIS );
    //protocol_.startHoming( A_AXIS );
    int status = -1;

    while( status ) {
      status = protocol_.getStatus( StatusBits, A_AXIS ) & STAT_HOMING;
      printf("Stat: %d\n", status & STAT_HOMING );
      fflush( stdout );
      boost::this_thread::sleep_for( boost::chrono::milliseconds( 500 ) );
    }
    //protocol_.startHoming( B_AXIS );
    protocol_.homingDone();
  }

private:
  ParallelProtocol &protocol_;
};

//-----------------------------------------------------------------------------
void ParallelProtocol::startHomingSequence( std::string sequence ) {
  homing_thread_ = new boost::thread( HomingSequencer( sequence, *this ) );
}

//-----------------------------------------------------------------------------
void ParallelProtocol::homingDone() {
 commanded_pos_.clear();
 homing_done_ = true;
}

//-----------------------------------------------------------------------------
void ParallelProtocol::setMaxSpeed( uint16_t spd, uint8_t axis ) {
  ConcurrentCmmd32 cmmds;
  for( uint32_t i = 1; i < AXIS_ALL; i <<= 1 )
    if( axis & i ) cmmds[i] = spd;
  sendSpdCmmds( cmmds );
}
//-----------------------------------------------------------------------------
void ParallelProtocol::updatePosition( const ConcurrentCmmd32 &cmmds ) {
  ConcurrentCmmd32::const_iterator it = cmmds.begin(), end = cmmds.end();
  for(; it != end; ++it) commanded_pos_[ it->first ] = it->second;
}

//-----------------------------------------------------------------------------
#define LOGPOS 1
#ifdef LOGPOS
std::ofstream lgfile("plot.txt"), lgpoints("points.txt");
std::map<uint8_t, double> last_pos, last_speed, last_cmd;
high_resolution_clock::time_point last_tstamp;
#endif
void ParallelProtocol::sendPosCmmds( const ConcurrentCmmd32 &cmmds ) {
  //uint32_t pins = axisToPins( axisMask( cmmds ) );
  /*
  ConcurrentCmmd ret = getParam( VelocityLimit, pins );
  ConcurrentCmmd::iterator kt = ret.begin(), kend = ret.end();
  for(; kt != kend; ++kt )
    printf("[%X] = [%d]\n", kt->first, kt->second );*/

  //setParam( ControlMode, CONTROLMODE_POSITON, pins ); // position mode

  ConcurrentCmmd64 pos_cmmds;
  ConcurrentCmmd32::const_iterator it = cmmds.begin(), end = cmmds.end();
#ifdef LOGPOS
  high_resolution_clock::time_point now = high_resolution_clock::now();
  double interval = boost::chrono::nanoseconds(now - last_tstamp).count() / 1e+9f;
  if( interval > 10 ) interval = 0;
  printf("Int: %f\n", interval);
  for( int i = 1; i < AXIS_ALL; i <<=1 ) {
    double offset = last_speed[i] * interval;
    if( last_pos[i] + offset > last_cmd[i] && last_speed[i] > 0 ) last_pos[i] = last_cmd[i];
    else if( last_pos[i] + offset < last_cmd[i] && last_speed[i] < 0 ) last_pos[i] = last_cmd[i];
    else last_pos[i] += offset;
    lgfile << last_pos[i] << " ";
    ConcurrentCmmd32::const_iterator found;
    if( (found = cmmds.find(i)) != end ) {
      if( found->second > last_pos[i] )      last_speed[i] =  fabs(last_speed[i]);
      else if( found->second < last_pos[i] ) last_speed[i] = -fabs(last_speed[i]);
      else                                   last_speed[i] = 0;
      last_cmd[i] = found->second;
    }
    lgpoints << last_cmd[i] << " ";
  }
  lgpoints << "\n";
  lgfile << "\n";
  last_tstamp = now;
#endif
  for(; it != end; ++it) {
    printf("p[%X] = [%d]\n", it->first, it->second );
    pos_cmmds[ it->first ] = spi_.graniteAbsTarget( it->second );
    last_cmmd_[it->first] = it->second;
  }

  uint64_t nope = spi_.nope();
  for( int i = 1; i < AXIS_ALL; i <<= 1) {
    if( pos_cmmds.find(i) == pos_cmmds.end() )
      pos_cmmds[i] = (nope << 32) | nope;
  }

  RetAxis rret = sendRawCommand64( pos_cmmds );
  updatePosition( cmmds );
  /*RetAxis::iterator jt = rret.begin(), jend = rret.end();
  for(; jt != jend; ++jt)
    printf("axis %X returned %X CRC match %X\n", jt->first, jt->second.data, jt->second.crc_match );*/
}

//-----------------------------------------------------------------------------
int32_t ParallelProtocol::getLastSentPos() {
  return last_cmmd_[X_AXIS];
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd ParallelProtocol::setParam( GraniteParams gp, uint32_t value, uint32_t pins ) {
  ConcurrentCmmd ret;

  RetAxis rraw = sendRawCommand64( spi_.graniteSetParam(gp, value), pins ); // this acttually returns the value
  RetAxis::iterator it = rraw.begin(), end = rraw.end();
  for(; it != end; ++it )
    if( it->second.crc_match )
      ret[ it->first ] = it->second.data;
    else
      std::cerr << "[set] CRC on axis " << int(it->first) << " did not match\n";

  return ret;
}


//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd ParallelProtocol::getParam( GraniteParams gp, uint32_t pins ) {
  ConcurrentCmmd ret;

  sendRawCommand32( spi_.getParam(gp), pins );
  sendRawCommand32( spi_.nope(), pins );
  RetAxis rraw = sendRawCommand32( spi_.nope(), pins ); // this acttually returns the value
  RetAxis::iterator it = rraw.begin(), end = rraw.end();
  for(; it != end; ++it )
    if( it->second.crc_match )
      ret[ it->first ] = it->second.data;
    else
      std::cerr << "[get] CRC on axis " << int(it->first) << " did not match\n";

  return ret;
}

//-----------------------------------------------------------------------------
void ParallelProtocol::sendSpdCmmds(const ConcurrentCmmd32 &cmmds ) {
  if( cmmds.empty() ) return;

  ConcurrentCmmd64 cmmd;
  ConcurrentCmmd32::const_iterator it = cmmds.begin(), end = cmmds.end();
#ifdef LOGPOS
  for( int i = 1; i < AXIS_ALL; i <<= 1 ) {
    ConcurrentCmmd32::const_iterator found;
    if( (found = cmmds.find(i)) != end )
      last_speed[i] = found->second / TO_RPM;
  }
#endif
  for(; it != end; ++it ) {
    printf("v[%X] = [%d]\n", it->first, it->second );
    cmmd[ it->first ] = spi_.graniteSetParam( VelocityLimit, it->second );
  }

  uint64_t nope = spi_.nope();
  for( int i = 1; i < AXIS_ALL; i <<= 1 ) {
    if( cmmd.find(i) == cmmd.end() )
      cmmd[i] = (nope << 32) | nope;
  }

  sendRawCommand64( cmmd );
}

//-----------------------------------------------------------------------------
int32_t ParallelProtocol::getStatus( GraniteParams param, uint8_t axis ) {
  reply_axis_ = axis;
  AbstractProtocol::ConcurrentCmmd ret;
  if( param == RawPosition ) {
    setParam( ReturnDataPayload, CAPTURE_RAW_POS, axisToPins(AXIS_ALL) );
    ret = getParam( ReturnDataPayload, axisToPins(AXIS_ALL) );
  } else
    ret = getParam( param, axisToPins(AXIS_ALL) );
  return ret.find(axis) != ret.end() ? ret[axis] : int32_t(~0);
}

//-----------------------------------------------------------------------------
void ParallelProtocol::sendAngularIncrement( AngularDirection dir, double spd, double inc ) {
  if( !homing_done_ ) return;
  printf("Angular increment direction %d, speed %f, incr %f\n", dir, spd, inc );
  ConcurrentCmmd32 speeds, pos;
  while( inc >  360 ) inc -= 360;
  while( inc < -360 ) inc += 360;
  int32_t pulses = 36*400 * inc/360,
          apos = commanded_pos_[A_AXIS],
          bpos = commanded_pos_[B_AXIS];
  speeds[A_AXIS] = speeds[B_AXIS] = spd;
  if( dir == ANGULAR_VERTICAL ) {
    pos[A_AXIS] = apos + pulses;
    pos[B_AXIS] = apos - pulses;
  } else if( dir == ANGULAR_HORIZONTAL ) {
    pos[A_AXIS] = apos + pulses;
    pos[B_AXIS] = apos + pulses;
  }

  sendSpdCmmds( speeds );
  sendPosCmmds( pos );
}

//-----------------------------------------------------------------------------
void ParallelProtocol::sendLinearIncrement( uint8_t axis, int32_t spd, int32_t inc ) {
  if( !homing_done_ ) return;
  printf(" Linear increment axis %d, speed %d, incr %d\n", axis, spd, inc);
  ConcurrentCmmd32 speeds, pos;
  int32_t cpos = 0;
  if( commanded_pos_.find(axis) != commanded_pos_.end() )
    cpos = commanded_pos_[axis];

  speeds[ axis ] = spd;
  pos[ axis ] = cpos + inc;

  sendSpdCmmds( speeds );
  sendPosCmmds( pos );
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
