#include <debug_protocol.h>

//-----------------------------------------------------------------------------
DebugProtocol::DebugProtocol() : AbstractProtocol( DEBUG ), posfile_("pos.dat") {

}
  
//-----------------------------------------------------------------------------
void DebugProtocol::startHoming( uint8_t ) {

}
  
//-----------------------------------------------------------------------------
void DebugProtocol::startHomingSequence( std::string ) {

}
  
//-----------------------------------------------------------------------------
void DebugProtocol::moveTo() {

}
  
//-----------------------------------------------------------------------------
void DebugProtocol::executeTrajectory() {

}
  
//-----------------------------------------------------------------------------
void DebugProtocol::finish() {

}
  
//-----------------------------------------------------------------------------
void DebugProtocol::setMaxSpeed( uint16_t spd, uint8_t axis ) {

}
  
//-----------------------------------------------------------------------------
void DebugProtocol::sendPosCmmds( const ConcurrentCmmd32 &cmmds ) {
  ConcurrentCmmd32 c = cmmds;
  posfile_ << c[X_AXIS] << " " << c[Y_AXIS] << "\n";
}
  
//-----------------------------------------------------------------------------
void DebugProtocol::sendSpdCmmds( const ConcurrentCmmd32 & ) {

}
  
//-----------------------------------------------------------------------------
int32_t DebugProtocol::getStatus( GraniteParams param, uint8_t axis ) {

}
  
//-----------------------------------------------------------------------------
Vector3I DebugProtocol::getLastSentPos() {
  return Vector3I(0,0,0);
}
  
//-----------------------------------------------------------------------------
void DebugProtocol::startTorch() {

}
  
//-----------------------------------------------------------------------------
void DebugProtocol::stopTorch() {

}
  
//-----------------------------------------------------------------------------
void DebugProtocol::sendAngularIncrement( AngularDirection dir, double spd, double inc ) {

}
  
//-----------------------------------------------------------------------------
void DebugProtocol::sendLinearIncrement(uint8_t axis, int32_t spd, int32_t inc ) {

}
  
//-----------------------------------------------------------------------------
