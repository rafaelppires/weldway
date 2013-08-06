#include <parallel_protocol.h>

//-----------------------------------------------------------------------------
ParallelProtocol::ParallelProtocol( uint16_t addr ) :
    AbstractProtocol( PARALLEL ), port_(addr) {
  port_.startSquareSignal( 16, 1000. ); // Pin 16 - 1kHz
}

//-----------------------------------------------------------------------------
void ParallelProtocol::startHoming( uint8_t axis ) {

}

//-----------------------------------------------------------------------------
void ParallelProtocol::moveTo() {

}

//-----------------------------------------------------------------------------
void ParallelProtocol::executeTrajectory() {

}

//-----------------------------------------------------------------------------
void ParallelProtocol::finish() {

}

//-----------------------------------------------------------------------------
