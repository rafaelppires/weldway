#include <master_communicator.h>

//-----------------------------------------------------------------------------
void MasterCommunicator::setupParallelPort( uint16_t addr ) {
  if( comm_ ) delete comm_.get();
  comm_.reset( new ParallelPort( addr ) );
  // start enable thread
}

//-----------------------------------------------------------------------------
