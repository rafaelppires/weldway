#include <windows.h>
#include <setupapi.h>
#include <parallel_port.h>
#include <cfgmgr32.h>
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>
using boost::chrono::nanoseconds;
using boost::chrono::high_resolution_clock;

//=============================================================================
MasterParallel::Inp32t MasterParallel::in  = 0;
MasterParallel::Out32t MasterParallel::out = 0;
//-----------------------------------------------------------------------------
MasterParallel::MasterParallel( uint16_t addr ) : addr_(addr),
    data_ready_(false), current_(2) {
  if( !in || !out ) {
    HINSTANCE lib = LoadLibrary( UNICODE ? (LPCTSTR)L"inpout32.dll" : (LPCTSTR)"inpout32.dll");
    if( !lib ) { std::cerr << "damn! no lib\n"; exit(0); }
    in  = (Inp32t)GetProcAddress(lib, "Inp32");
    out = (Out32t)GetProcAddress(lib, "Out32");
    if( !in || !out ) { std::cerr << "damn! no funcs IN: " << in << " OUT: " << out << "\n"; exit(0); }
  }
}

#include <fstream>
std::ofstream logfile("siglog.dat");
//-----------------------------------------------------------------------------
void MasterParallel::iteration() {
  boost::unique_lock<boost::mutex> lock(mutex_);
  while( !data_ready_ )
    condition_.wait(lock);


  out( addr_, decodeBase(current_) ); // base addres (write only pins)
  out( addr_ + 2, decodeBase2(current_) ); // base address + 2 (read/write pins)

  /*if( log_ ) {
    char out[1024];
    sprintf( out, "%d %d %d\n", (current_&0x10000)>>16, (current_&4) >> 2, (current_&8) >> 3  );
    logfile << out;
  }*/

  data_ready_ = false;
}
//-----------------------------------------------------------------------------
void MasterParallel::invertPin( uint8_t pinidx ) {
  if( !validPinIdx(pinidx) ) return;
  writePins( ~current_, 1 << pinidx );
}

//-----------------------------------------------------------------------------
void MasterParallel::invertPinSync( uint8_t pinidx ) {
  if( !validPinIdx(pinidx) ) return;
  writePinsSync( ~current_, 1 << pinidx );
}

//-----------------------------------------------------------------------------
void MasterParallel::writePins(uint32_t value, uint32_t mask ) {
  mask &= wmask;
  {
    boost::lock_guard<boost::mutex> lock(mutex_);
    uint32_t prev = current_;
    current_ &= ~mask;
    current_ |= value & mask;
    //if( prev != current_ )
    data_ready_ = true;
  }
  condition_.notify_all();
}
//-----------------------------------------------------------------------------
void MasterParallel::writePinsSync( uint32_t value, uint32_t mask ) {
  writePins( value, mask );
  bool done = false;
  do {
    boost::this_thread::yield();

    mutex_.lock();
    done = !data_ready_;
    mutex_.unlock();
  } while( !done );
}

//-----------------------------------------------------------------------------
uint32_t MasterParallel::readPins( uint32_t mask ) {
  uint32_t ret = 0;
  uint16_t input;
  mask &= rmask;
  if( mask & romsk ) {
    input = in( addr_ + 1 );
    ret |= input & PPIN10 ? 1<<10 : 0;
    ret |= input & PPIN11 ? 1<<11 : 0;
    ret |= input & PPIN12 ? 1<<12 : 0;
    ret |= input & PPIN13 ? 1<<13 : 0;
    ret |= input & PPIN15 ? 1<<15 : 0;
  }

  if( mask & ~romsk ) {
    input = in( addr_ + 2 );
    ret |= input & PPIN01 ? 1<<1  : 0;
    ret |= input & PPIN14 ? 1<<14 : 0;
    ret |= input & PPIN16 ? 1<<16 : 0;
    ret |= input & PPIN17 ? 1<<17 : 0;
  }

  return ret & mask;
}

//-----------------------------------------------------------------------------
uint16_t MasterParallel::decodeBase( uint32_t encoded ) {
  uint16_t ret = 0;
  ret |= encoded & 1<<2 ? PPIN02 : 0;
  ret |= encoded & 1<<3 ? PPIN03 : 0;
  ret |= encoded & 1<<4 ? PPIN04 : 0;
  ret |= encoded & 1<<5 ? PPIN05 : 0;
  ret |= encoded & 1<<6 ? PPIN06 : 0;
  ret |= encoded & 1<<7 ? PPIN07 : 0;
  ret |= encoded & 1<<8 ? PPIN08 : 0;
  ret |= encoded & 1<<9 ? PPIN09 : 0;
  return ret;
}

//-----------------------------------------------------------------------------
uint16_t MasterParallel::decodeBase2( uint32_t encoded ) {
  uint16_t ret = 0;
  ret |= encoded & 1<<1  ? PPIN01 : 0;
  ret |= encoded & 1<<14 ? PPIN14 : 0;
  ret |= encoded & 1<<16 ? PPIN16 : 0;
  ret |= encoded & 1<<17 ? PPIN17 : 0;
  return ret;
}

//=============================================================================
SquareSigGen::SquareSigGen( ParallelPort &p, uint8_t pin_idx, double freq ) :
    port_(p), pin_(pin_idx),
    period_(1e+6f/(2*freq)) {} // microseconds

//-----------------------------------------------------------------------------
void SquareSigGen::iteration() {
  high_resolution_clock::time_point now, start;
  nanoseconds diff;
  start = high_resolution_clock::now();
  do {
    boost::this_thread::yield();
    now = high_resolution_clock::now();
    diff = now - start;
  } while( diff.count()/1e3f < period_ );

  port_.invertPin( pin_ );
}

//=============================================================================
ReadingThread::ReadingThread( ParallelPort &p, uint8_t pin_idx, double s, ReadCallbackType f ) :
    pin_(pin_idx), period_(s*1e+6f), callback_(f), port_(p), value_(false), once_(true) {
  callback_( false );
}

//-----------------------------------------------------------------------------
void ReadingThread::iteration() {
  uint32_t mask = 1 << pin_;
  bool now = bool(mask & port_.readPins( mask ));

  if( once_ ) {
    callback_( now );
    value_ = now;
    once_ = false;
  } else if( now != value_ ) {
    callback_( now );
    value_ = now;
  }

  boost::this_thread::sleep_for( boost::chrono::microseconds(uint32_t(period_)) );
}

//=============================================================================
ParallelPort::ParallelPort(uint16_t addr) : master_parallel_(addr){
  master_thread_  = new boost::thread( boost::ref(master_parallel_) );
}

//-----------------------------------------------------------------------------
std::string ParallelPort::toStdString( char *buf ) {
  std::string ret( buf );
  if( UNICODE ) {
    std::wstring ws((wchar_t*)buf);
    ret = std::string(ws.begin(),ws.end());
  }
  return ret;
}

//-----------------------------------------------------------------------------
void ParallelPort::invertPin( uint8_t pin_idx ) {
  master_parallel_.invertPin( pin_idx );
}

//-----------------------------------------------------------------------------
void ParallelPort::invertPinSync( uint8_t pin_idx ) {
  master_parallel_.invertPinSync( pin_idx );
}

//-----------------------------------------------------------------------------
void ParallelPort::writePins( uint32_t value, uint32_t mask ) {
  master_parallel_.writePins( value, mask );
}

//-----------------------------------------------------------------------------
uint32_t ParallelPort::readPins( uint32_t mask ) {
  return master_parallel_.readPins( mask );
}

//-----------------------------------------------------------------------------
bool ParallelPort::invertedPin( uint8_t pinidx ) {
  return pinidx == 1 || pinidx == 9 || pinidx == 11 || pinidx == 17;
}

//-----------------------------------------------------------------------------
void ParallelPort::setHighPinSync( uint8_t pinidx ) {
  uint32_t v = 1 << pinidx,
           value = invertedPin(pinidx) ? 0 : v;
  master_parallel_.writePinsSync( value, v );
}

//-----------------------------------------------------------------------------
void ParallelPort::setLowPinSync( uint8_t pinidx ) {
  uint32_t v = 1 << pinidx,
           value = invertedPin(pinidx) ? v : 0;
  master_parallel_.writePinsSync( value, v );
}

//-----------------------------------------------------------------------------
void ParallelPort::writePinsSync( uint32_t value, uint32_t mask ) {
  master_parallel_.writePinsSync( value, mask );
}

//-----------------------------------------------------------------------------
void ParallelPort::startSquareSignal( uint8_t pin_idx, double freq ) {
  square_threads_[ pin_idx ] =
            new boost::thread( SquareSigGen( *this, pin_idx, freq ) );
}

//-----------------------------------------------------------------------------
void ParallelPort::startReadingPin( uint8_t pin_idx, double period ,
                                    ReadCallbackType f ) {
  reading_threads_[ pin_idx ] =
          new boost::thread( ReadingThread( *this, pin_idx, period, f ) );
}

//-----------------------------------------------------------------------------
void ParallelPort::startLogging() {
  master_parallel_.setLogging( true );
}

//-----------------------------------------------------------------------------
void ParallelPort::stopLogging() {
  master_parallel_.setLogging( false );
}

//-----------------------------------------------------------------------------
// Checks with windows the available parallel ports and their addresses
//-----------------------------------------------------------------------------
ParallelPort::ParallelList ParallelPort::list() {
  ParallelList ret;

  HDEVINFO hdev_info = SetupDiGetClassDevs( 0L, 0L, 0L,
                           DIGCF_PRESENT | DIGCF_ALLCLASSES | DIGCF_PROFILE ) ;  
  if( hdev_info == (void*)-1 ) {
    return ret;
  }
  
  uint32_t idx = 0;
  SP_DEVINFO_DATA spdev_info = {0};  
  spdev_info.cbSize = sizeof(SP_DEVINFO_DATA);  
  char szBuf[ 2048 ]; szBuf[0] = 0; DWORD rs;
  while( SetupDiEnumDeviceInfo( hdev_info, idx++, &spdev_info ) ) {
    bool ok = SetupDiGetDeviceRegistryProperty( hdev_info, &spdev_info, SPDRP_CLASS, 0L,
                                                         (PBYTE)szBuf, 2048, &rs);
    if( toStdString(szBuf) != "Ports" )
      continue;

    if( !ok )
      continue;

    SetupDiGetClassDescription(&spdev_info.ClassGuid, (PWSTR)szBuf, 2048, 0);
    SetupDiGetDeviceRegistryProperty( hdev_info, &spdev_info, SPDRP_FRIENDLYNAME, 0L, (PBYTE)szBuf, 2048, 0);

    std::string friendly_name = toStdString( szBuf );
    SetupDiGetDeviceRegistryProperty( hdev_info, &spdev_info, SPDRP_SERVICE, 0L, (PBYTE)szBuf, 2048, 0);

    if( !boost::iequals(toStdString( szBuf ), "parport") )
      continue;

    LOG_CONF  firstLogConf, nextLogConf, rdPrevResDes;
    CONFIGRET cmdRet;
    // ALLOC_LOG_CONF: A resource list identifying resources currently in use
    // by a device instance. Only one allocated configuration can exist for 
    // each device instance.
    cmdRet = CM_Get_First_Log_Conf( &firstLogConf, spdev_info.DevInst, ALLOC_LOG_CONF);
    if( cmdRet != CR_SUCCESS )
      continue;

    cmdRet = CM_Get_Next_Res_Des(&nextLogConf, firstLogConf, ResType_IO, 0L, 0L );
    if( cmdRet != CR_SUCCESS ) {
      CM_Free_Res_Des_Handle( firstLogConf );
      continue;
    }

    while( true ) {
      ULONG ulSize = 0;
      cmdRet = CM_Get_Res_Des_Data_Size(&ulSize, nextLogConf, 0L);
      if( cmdRet != CR_SUCCESS )
        break;

      cmdRet = CM_Get_Res_Des_Data(nextLogConf, szBuf, std::min<ULONG>(ulSize,sizeof(szBuf)), 0L);
      if( cmdRet != CR_SUCCESS )
        break;

      IO_DES *io = (IO_DES*)szBuf;

      ret.push_back( std::make_pair( friendly_name, uint32_t(io->IOD_Alloc_Base) ) );

      rdPrevResDes = 0;
      cmdRet = CM_Get_Next_Res_Des(&rdPrevResDes, nextLogConf, ResType_IO, 0L, 0L);
      
      if (cmdRet != CR_SUCCESS)
         break;
      else {
        CM_Free_Res_Des_Handle(nextLogConf);
        nextLogConf = rdPrevResDes;
      };
    }
    CM_Free_Res_Des_Handle( nextLogConf );
    CM_Free_Res_Des_Handle( firstLogConf );
  }

  return ret;
}

//-----------------------------------------------------------------------------

