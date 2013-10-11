#include <master_communicator.h>
#include <parallel_protocol.h>
#include <boost/chrono.hpp>
using boost::chrono::high_resolution_clock;
using boost::chrono::nanoseconds;
using boost::chrono::milliseconds;

//-----------------------------------------------------------------------------
void TrajectoryExecuter::operator()() {
  bool spd = trajectory_->controlMode() & VELOCITY,
       pos = trajectory_->controlMode() & POSITION;
  high_resolution_clock::time_point now, start;

  FILE *log = fopen("log.txt", "w");

  uint32_t interval;
  start = high_resolution_clock::now();
  while( !trajectory_->finished() ) {
    now = high_resolution_clock::now();
    fprintf(log, "int %f ", (now - start).count() / 1e+6f );
    if( spd ) {
      AbstractProtocol::ConcurrentCmmd32 cmmds = trajectory_->speed();
      comm_->sendSpdCmmds( cmmds );
    }

    if( pos ) {
      AbstractProtocol::ConcurrentCmmd32 cmmds = trajectory_->position();
      comm_->sendPosCmmds( cmmds );
    }
    //goto finish_traj;
    start = high_resolution_clock::now();
    milliseconds interv = trajectory_->interval() - boost::chrono::duration_cast<milliseconds>(start - now);
    fprintf(log, "cmd %f %d ms\n", (start - now).count() / 1e+6f, interv.count() );
    boost::this_thread::sleep_for( interv );
  }
finish_traj:
  {
  boost::lock_guard<boost::mutex> lock(finish_mutex_);
  finished_ = true;
  }

  fclose(log);
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
  if( !comm_ ) return false;
  if( trajectory_executer_ && !trajectory_executer_->finished() ) return false;
  delete trajectory_executer_;
  trajectory_executer_ = new TrajectoryExecuter( at, comm_ );
  thread_executer_.reset( new boost::thread( boost::ref(*trajectory_executer_) ) );
  return true;
}
