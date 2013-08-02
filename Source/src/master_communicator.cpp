#include <master_communicator.h>
#include <parallel_port.h>

//-----------------------------------------------------------------------------
void MasterCommunicator::setupParallelPort( uint16_t addr ) {
  if( comm_ ) {
    comm_->finish();
    delete comm_.get();
  }

  ParallelPort *pp;
  comm_.reset( pp = new ParallelPort( addr ) );
  pp->startSquareSignal( 16, 1000. ); // Pin 16 - 1kHz
}

//-----------------------------------------------------------------------------
