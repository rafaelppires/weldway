#ifndef _MASTER_COMMUNICATOR_H_
#define _MASTER_COMMUNICATOR_H_

#include <trajectory.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#define X_AXIS   0x01
#define Y_AXIS   0x02
#define Z_AXIS   0x04
#define A_AXIS   0x08
#define B_AXIS   0x10
#define AXIS_ALL 0x1F
#define AXIS_CNT 5

//-----------------------------------------------------------------------------
class TrajectoryExecuter {
public:
  TrajectoryExecuter( AbstractTrajectory t, boost::shared_ptr< AbstractProtocol > comm )
      : trajectory_(t), comm_(comm), finished_(false) {}

  void operator()();
  bool finished();

private:
  AbstractTrajectory trajectory_;
  boost::shared_ptr< AbstractProtocol > comm_;
  boost::mutex finish_mutex_;
  bool finished_;
};

//-----------------------------------------------------------------------------
class MasterCommunicator {
public:
  // Singleton
  static MasterCommunicator& getInstance() {
    static MasterCommunicator instance;
    return instance;
  }

  void setupParallelPort( uint16_t addr );
  bool startHoming( uint8_t axis );
  bool setMaxSpeed(uint16_t speed_rpm, uint8_t axis);
  bool sendPosCmmds(AbstractProtocol::ConcurrentCmmd & cmmds);
  bool executeTrajectory(AbstractTrajectory & );
  
private:
  MasterCommunicator() {}
  MasterCommunicator(MasterCommunicator const&);
  void operator=(MasterCommunicator const&);

  boost::shared_ptr< AbstractProtocol > comm_;
  TrajectoryExecuter *trajectory_executer_;
};

#endif
