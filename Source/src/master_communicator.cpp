#include <master_communicator.h>
#include <parallel_protocol.h>

//-----------------------------------------------------------------------------
void TrajectoryExecuter::operator()() {
  bool spd = trajectory_->controlMode() & VELOCITY,
       pos = trajectory_->controlMode() & POSITION;

  uint32_t interval;
  while( !trajectory_->finished() ) {
    if( spd ) comm_->setMaxSpeed( trajectory_->speed(), X_AXIS );
    if( pos ) {
      AbstractProtocol::ConcurrentCmmd cmmds = trajectory_->position();
      comm_->sendPosCmmds( cmmds );
    }
    boost::this_thread::sleep_for( trajectory_->interval() );
  }

  {
  boost::lock_guard<boost::mutex> lock(finish_mutex_);
  finished_ = true;
  }
}

//-----------------------------------------------------------------------------
bool TrajectoryExecuter::finished() {
   bool ret;
   {
     boost::lock_guard<boost::mutex> lock(finish_mutex_);
     ret = finished_;
   }
   return ret;
 }

//-----------------------------------------------------------------------------
void MasterCommunicator::setupParallelPort( uint16_t addr ) {
  if( comm_ ) {
    comm_->finish();
    delete comm_.get();
  }

  comm_.reset( new ParallelProtocol( addr ) );
}

//-----------------------------------------------------------------------------
bool MasterCommunicator::startHoming( uint8_t axis ) {
  if( !comm_ ) return false;
  comm_->startHoming( axis );
  return true;
}

//-----------------------------------------------------------------------------
bool MasterCommunicator::setMaxSpeed( uint16_t speed_rpm, uint8_t axis ) {
  if( !comm_ ) return false;
  comm_->setMaxSpeed( speed_rpm, axis );
  return true;
}

//-----------------------------------------------------------------------------
bool MasterCommunicator::sendPosCmmds( AbstractProtocol::ConcurrentCmmd &cmmds ) {
  if( !comm_ ) return false;
  comm_->sendPosCmmds( cmmds );
  return true;
}

//-----------------------------------------------------------------------------
bool MasterCommunicator::executeTrajectory( AbsTrajectoryPtr at ) {
  if( !comm_ ) return false;
  if( trajectory_executer_ && !trajectory_executer_->finished() ) return false;
  delete trajectory_executer_;
  trajectory_executer_ = new TrajectoryExecuter( at, comm_ );
  thread_executer_.reset( new boost::thread( boost::ref(*trajectory_executer_) ) );
  return true;
}
