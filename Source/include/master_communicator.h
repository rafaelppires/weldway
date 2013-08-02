#ifndef _MASTER_COMMUNICATOR_H_
#define _MASTER_COMMUNICATOR_H_

#include <stdint.h>
#include <boost/shared_ptr.hpp>

enum CommType {
  PARALLEL,
  USB_HUB,
  USB_PROXY
};

class AbstractCommunication {
public:
  AbstractCommunication( CommType t ) : type_(t) {}
  virtual void finish() {}
private:
  CommType type_;
};

class MasterCommunicator {
public:
  // Singleton
  static MasterCommunicator& getInstance() {
    static MasterCommunicator instance;
    return instance;
  }

  void setupParallelPort( uint16_t addr );
  
private:
  MasterCommunicator() {}
  MasterCommunicator(MasterCommunicator const&);
  void operator=(MasterCommunicator const&);

  boost::shared_ptr< AbstractCommunication > comm_;
};

#endif
