#ifndef _MASTER_COMMUNICATOR_H_
#define _MASTER_COMMUNICATOR_H_

#include <trajectory.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <granite_spi_interface.h>

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class TrajectoryExecuter {
public:
  TrajectoryExecuter( AbsTrajectoryPtr t, boost::shared_ptr< AbstractProtocol > comm )
      : trajectory_(t), comm_(comm), finished_(false) {}

  void operator()();
  bool finished();
  void cancel();
  void setLimits(const Vector3US &init, const Vector3US &final );
  void setCurrent( int32_t last );

private:
  AbsTrajectoryPtr trajectory_;
  boost::shared_ptr< AbstractProtocol > comm_;
  boost::mutex finish_mutex_;
  bool finished_;
  Vector3US trajectory_init_, trajectory_final_;
  int32_t current_pos_;
};

//-----------------------------------------------------------------------------
class MasterCommunicator {
public:
  // Singleton
  static MasterCommunicator& getInstance() {
    static MasterCommunicator instance;
    return instance;
  }

  // Queries
  bool busy();
  int32_t getStatus( GraniteParams, uint8_t );

  // Actions
  bool startHoming( uint8_t axis );
  bool startHomingSequence( std::string sequence );
  bool sendPosCmmds(AbstractProtocol::ConcurrentCmmd32 &cmmds);
  bool executeTrajectory( AbsTrajectoryPtr );
  void cancel();
  bool sendAngularIncrement( AngularDirection, double spd, double inc );
  bool sendIncrement( uint8_t axis, double spd, double inc );

  // Setup
  void setupParallelPort( uint16_t addr );
  void setupDebug();
  void setLimits(const Vector3US &init, const Vector3US &final );
  bool setMaxSpeed(uint16_t speed_rpm, uint8_t axis);
  
private:
  MasterCommunicator() {}
  MasterCommunicator(MasterCommunicator const&);
  void operator=(MasterCommunicator const&);
  void emergencyCallback();

  boost::shared_ptr< AbstractProtocol > comm_;
  TrajectoryExecuter *trajectory_executer_;
  boost::shared_ptr<boost::thread> thread_executer_;
  Vector3US trajectory_init_, trajectory_final_;
};

#endif
