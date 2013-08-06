#ifndef _PARALLEL_PROTOCOL_H_
#define _PARALLEL_PROTOCOL_H_

#include <master_communicator.h>
#include <parallel_port.h>

class ParallelProtocol : public AbstractProtocol {
public:
  ParallelProtocol( uint16_t addr );
  virtual void startHoming( uint8_t );
  virtual void moveTo();
  virtual void executeTrajectory();
  virtual void finish();

private:
  ParallelPort port_;
};

#endif
