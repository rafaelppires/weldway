#ifndef _MASTER_COMMUNICATOR_H_
#define _MASTER_COMMUNICATOR_H_

#include <trajectory.h>
#include <protocol.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <granite_spi_interface.h>
#include <trajectory_executer.h>

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
  Vector3D currentPosition();

  // Actions
  bool startHoming( uint8_t axis );
  bool startHomingSequence( std::string sequence );
  bool sendPosCmmds(AbstractProtocol::ConcurrentCmmd32 &cmmds);
  bool executeTrajectory( AbsTrajectoryPtr );
  void cancel();
  bool sendAngularIncrement( AngularDirection, double spd, double inc );
  bool sendLinearIncrement( uint8_t axis, double spd, double inc );
  void setAngularOffset( double angle );
  bool sendLinearCorrection( uint8_t axis, double inc );

  // Setup
  void setupParallelPort( uint16_t addr );
  void setupDebug();
  void setLimits(const Vector3I &init, const Vector3I &final );
  bool setMaxSpeed(uint16_t speed_rpm, uint8_t axis);
  void setProgressCallback( boost::function<void(double)> cb );
  void setEmergencyCallback( EmergencyCallbackType );
  
private:
  MasterCommunicator() {}
  MasterCommunicator(MasterCommunicator const&);
  void operator=(MasterCommunicator const&);
  void emergencyCallback( bool );

  boost::shared_ptr< AbstractProtocol > comm_;
  TrajectoryExecuter *trajectory_executer_;
  boost::shared_ptr<boost::thread> thread_executer_;
  Vector3I trajectory_init_, trajectory_final_;
  double angular_offset_;
  boost::function<void(double)> progress_callback_;
  EmergencyCallbackType emergency_callback_;
};

#endif
