#include <master_communicator.h>
#include <parallel_protocol.h>

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
void MasterCommunicator::setMaxSpeed( uint16_t, uint8_t ) {

}

//-----------------------------------------------------------------------------
void MasterCommunicator::sendPosCmmds( ConcurrentPosCmmd & ) {

}

//-----------------------------------------------------------------------------
