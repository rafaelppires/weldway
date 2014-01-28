#include <master_communicator.h>
#include <parallel_protocol.h>
#include <debug_protocol.h>
#include <boost/chrono.hpp>
using boost::chrono::high_resolution_clock;
using boost::chrono::nanoseconds;
using boost::chrono::milliseconds;

//-----------------------------------------------------------------------------
void MasterCommunicator::setupParallelPort( uint16_t addr ) {
  if( comm_ )
    comm_->finish();

  ParallelProtocol *p;
  comm_.reset( p = new ParallelProtocol( addr,
            boost::bind( &MasterCommunicator::emergencyCallback, this, _1 )) );
}

//-----------------------------------------------------------------------------
void MasterCommunicator::setupDebug() {
  if( comm_ )
    comm_->finish();

  comm_.reset( new DebugProtocol );
}
//-----------------------------------------------------------------------------
void MasterCommunicator::setEmergencyCallback( EmergencyCallbackType cb ) {
  emergency_callback_ = cb;
}

//-----------------------------------------------------------------------------
void MasterCommunicator::emergencyCallback( bool state ) {
  if( emergency_callback_ ) emergency_callback_(state);
  if( state ) cancel();
}

//-----------------------------------------------------------------------------
bool MasterCommunicator::startHoming( uint8_t axis ) {
  if( !comm_ ) return false;
  comm_->startHoming( axis );
  return true;
}

//-----------------------------------------------------------------------------
bool MasterCommunicator::startHomingSequence( std::string sequence ) {
  if( !comm_ ) return false;
  comm_->startHomingSequence( sequence );
  return true;
}

//-----------------------------------------------------------------------------
bool MasterCommunicator::setMaxSpeed( uint16_t speed_rpm, uint8_t axis ) {
  if( !comm_ ) return false;
  comm_->setMaxSpeed( speed_rpm, axis );
  return true;
}

//-----------------------------------------------------------------------------
bool MasterCommunicator::sendPosCmmds( AbstractProtocol::ConcurrentCmmd32 &cmmds ) {
  if( !comm_ ) return false;
  comm_->sendPosCmmds( cmmds );
  return true;
}

//-----------------------------------------------------------------------------
int32_t MasterCommunicator::getStatus( GraniteParams p, uint8_t axis ) {
  if( !comm_ ) return ~0;
  return comm_->getStatus(p, axis);
}

//-----------------------------------------------------------------------------
bool MasterCommunicator::executeTrajectory( AbsTrajectoryPtr at ) {
  if( !comm_ || busy() ) return false;
  delete trajectory_executer_;
  trajectory_executer_ = new TrajectoryExecuter( at, comm_ );
  trajectory_executer_->setCurrent( comm_->getLastSentPos() );
  trajectory_executer_->setLimits( trajectory_init_, trajectory_final_ );
  trajectory_executer_->setAngularOffset( angular_offset_ );
  trajectory_executer_->setProgressCallback( progress_callback_ );
  thread_executer_.reset( new boost::thread( boost::ref(*trajectory_executer_) ) );
  return true;
}

//-----------------------------------------------------------------------------
bool MasterCommunicator::busy() {
  if( trajectory_executer_ )
    return !trajectory_executer_->finished();
  return false;
}

//-----------------------------------------------------------------------------
void MasterCommunicator::cancel() {
  if( trajectory_executer_ )
    trajectory_executer_->cancel();
}

//-----------------------------------------------------------------------------
void MasterCommunicator::setLimits(const Vector3I &init, const Vector3I &final ) {
  trajectory_init_ = init;
  trajectory_final_ = final;
}

//-----------------------------------------------------------------------------
void MasterCommunicator::setProgressCallback( boost::function<void(double)> cb ) {
  progress_callback_ = cb;
}

//-----------------------------------------------------------------------------
Vector3D MasterCommunicator::currentPosition() {
  Vector3D ret;
  if( !comm_ ) return ret;
  ret = comm_->getLastSentPos();
  return ret / TO_PULSES;
}

//-----------------------------------------------------------------------------
void MasterCommunicator::setAngularOffset( double angle ) {
  angular_offset_ = angle * acos(-1) / 180;
}

//-----------------------------------------------------------------------------
bool MasterCommunicator::sendAngularIncrement( AngularDirection dir, double spd, double inc ) {
  if( !comm_ || busy() ) return false;
  comm_->sendAngularIncrement(dir,spd,inc);
  return true;
}

//-----------------------------------------------------------------------------
bool MasterCommunicator::sendLinearIncrement( uint8_t axis, double spd, double inc ) {
  if( !comm_ || busy() ) return false;
  comm_->sendLinearIncrement(axis,spd,inc);
  return true;
}

//-----------------------------------------------------------------------------
