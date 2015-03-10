#include <debug_protocol.h>
#include <granite/vsd_cmd.h>

//-----------------------------------------------------------------------------
DebugProtocol::DebugProtocol() : AbstractProtocol( DEBUG ), posfile_("pos.dat") {

}
  
//-----------------------------------------------------------------------------
DebugProtocol::~DebugProtocol() {
  posfile_.flush();
  posfile_.close();
}

//-----------------------------------------------------------------------------
void DebugProtocol::startHoming( uint8_t ) {
  homingFinished();
}
  
//-----------------------------------------------------------------------------
void DebugProtocol::startHomingSequence() {
  homingFinished();
}
  
//-----------------------------------------------------------------------------
void DebugProtocol::setMaxSpeed( uint16_t spd, uint8_t axis ) {

}
  
//-----------------------------------------------------------------------------
void DebugProtocol::sendPosCmmds( const ConcurrentCmmd32 &cmmds ) {
  ConcurrentCmmd32::const_iterator it = cmmds.begin(), end = cmmds.end();
  for(; it != end; ++it ) lastcmmd_pos_[it->first] = it->second;
  posfile_ << lastcmmd_pos_[X_AXIS] << " " << lastcmmd_pos_[Y_AXIS] << " " << lastcmmd_pos_[Z_AXIS] << " " << lastcmmd_pos_[A_AXIS] << " " << lastcmmd_pos_[B_AXIS]  << "\n";
  posfile_.flush();
}

//-----------------------------------------------------------------------------
void DebugProtocol::setSinc( uint8_t v ) {
  if (v)
    posfile_ << "+\n";
  else
    posfile_ << "-\n";
}

//-----------------------------------------------------------------------------
void DebugProtocol::sendSpdCmmds( const ConcurrentCmmd32 & ) {

}
  
//-----------------------------------------------------------------------------
int32_t DebugProtocol::getStatus( GraniteParams param, uint8_t axis ) {
  return STAT_TARGET_REACHED;
}

//-----------------------------------------------------------------------------
void DebugProtocol::startTorch() {

}
  
//-----------------------------------------------------------------------------
void DebugProtocol::stopTorch() {

}

//-----------------------------------------------------------------------------
