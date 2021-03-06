#ifndef _PARALLEL_COMM_H_
#define _PARALLEL_COMM_H_

#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <boost/thread.hpp>

// BASE (Data)
#define PPIN02 0x01 // OUT
#define PPIN03 0x02 // OUT
#define PPIN04 0x04 // OUT
#define PPIN05 0x08 // OUT
#define PPIN06 0x10 // OUT
#define PPIN07 0x20 // OUT
#define PPIN08 0x40 // OUT
#define PPIN09 0x80 // OUT - INVERTED

// BASE + 1 (Status)
#define PPIN10 0x40 // IN
#define PPIN11 0x80 // IN - INVERTED
#define PPIN12 0x20 // IN
#define PPIN13 0x10 // IN
#define PPIN15 0x08 // IN

// BASE + 2 (Control)
#define PPIN01 0x01 // IN/OUT - INVERTED
#define PPIN14 0x02 // IN/OUT
#define PPIN16 0x04 // IN/OUT
#define PPIN17 0x08 // IN/OUT - INVERTED

#define PINCOUNT 17

//-----------------------------------------------------------------------------
class StoppableThread {
public:
  StoppableThread() : finished_(false) {}

  void operator()() {
    mutex_.lock();
    while( !finished_ ) {
      mutex_.unlock();
      iteration();
      mutex_.lock();
    }
    mutex_.unlock();
  }

  void finish() {
    boost::lock_guard<boost::mutex> lock(mutex_);
    finished_ = true;
  }

private:
  virtual void iteration() = 0;
  boost::mutex mutex_;
  bool finished_;
};

//-----------------------------------------------------------------------------
class MasterParallel : public StoppableThread {
  static const uint32_t rmask = 0x3FC02, // PINS 1;10-17
                        wmask = 0x343FE, // PINS 1-9;14;16;17
                        romsk = rmask & ~wmask, // PINS 10-13;15
                        womsk = wmask & ~rmask, // PINS 2-9
                        pinall = rmask | wmask;
public:
  MasterParallel( uint16_t addr );
  void iteration();

  void invertPin(uint8_t pinidx);
  void invertPinSync(uint8_t pinidx);

  void writePins( uint32_t value, uint32_t mask );
  void writePinsSync( uint32_t value, uint32_t mask );

  uint32_t readPins( uint32_t mask );
  void setLogging( bool v ) { log_ = v; }

private:
  inline bool validPinIdx( uint8_t i ) { return i && i <= PINCOUNT; }
  uint16_t decodeBase( uint32_t encoded );
  uint16_t decodeBase2( uint32_t encoded );
  bool data_ready_;
  boost::condition_variable condition_;
  boost::mutex mutex_;
  uint32_t current_;
  uint16_t addr_;
  bool log_;

  typedef short _stdcall (*Inp32t)(short);
  typedef void _stdcall (*Out32t)(short, short);
  static Inp32t in;
  static Out32t out;
};

//-----------------------------------------------------------------------------
typedef boost::function< void(bool) > ReadCallbackType;
//-----------------------------------------------------------------------------
class ParallelPort {
public:
  typedef std::vector< std::pair< std::string, uint32_t > > ParallelList;

  ParallelPort( uint16_t addr );

  // Checks with windows the available parallel ports and their addresses
  static ParallelList list();

  void sendBatch( std::vector<uint32_t> &, uint32_t mask );
  void invertPin( uint8_t pin_idx );
  void invertPinSync( uint8_t pin_idx );
  void writePins( uint32_t value, uint32_t mask );
  void startSquareSignal( uint8_t pin_idx, double freq );
  void startReadingPin( uint8_t pin_idx, double period , ReadCallbackType f );
  uint32_t readPins( uint32_t mask );

  void setHighPinSync( uint8_t pinidx );
  void setLowPinSync( uint8_t pinidx );
  void writePinsSync( uint32_t value, uint32_t mask );
  void startLogging();
  void stopLogging();
private:
  bool invertedPin( uint8_t pinidx );
  static std::string toStdString( char * );
  std::map< uint8_t, boost::thread* > square_threads_;
  std::map< uint8_t, boost::thread* > reading_threads_;
  MasterParallel master_parallel_;
  boost::thread *master_thread_;
};

//-----------------------------------------------------------------------------
class SquareSigGen : public StoppableThread {
public:
  SquareSigGen( const SquareSigGen &s ) : port_(s.port_), pin_(s.pin_), period_(s.period_) {}
  SquareSigGen( ParallelPort &p, uint8_t pin_idx, double freq );
  void iteration();

private:
  uint8_t pin_;
  double period_;
  ParallelPort &port_;
};

//-----------------------------------------------------------------------------
class ReadingThread : public StoppableThread {
public:
  ReadingThread( const ReadingThread &r ) : port_(r.port_), pin_(r.pin_),
                 period_(r.period_), callback_(r.callback_), value_(r.value_) {}
  ReadingThread( ParallelPort &p, uint8_t pin_idx, double ms, ReadCallbackType f );
  void iteration();
private:
  bool value_, once_;
  uint8_t pin_;
  double period_;
  ReadCallbackType callback_;
  ParallelPort &port_;
};

#endif
