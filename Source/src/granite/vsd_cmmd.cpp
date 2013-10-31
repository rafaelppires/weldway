
#include <granite/vsd_cmd.h>

std::string faultString( uint16_t status ) {
  std::string ret;
  if( status == 0 )
    return "Sem falhas";
  if( status == uint16_t(~0) )
    return "Sem resposta";
  if( status & FLT_INVALIDCMD )
    ret += "Comando inválido.";
  if( status & FLT_FOLLOWERROR )
    ret += "Seguimento.";
  if( status & FLT_OVERCURRENT )
    ret += "Sobrecorrente.";
  if( status & FLT_COMMUNICATION )
    ret += "Comunicação.";
  if( status & FLT_ENCODER )
    ret += "Encoder.";
  if( status & FLT_OVERTEMP )
    ret += "Temperatura.";
  if( status & FLT_UNDERVOLTAGE )
    ret += "Subtensão.";
  if( status & FLT_OVERVOLTAGE )
    ret += "Sobretensão.";
  if( status & FLT_PROGRAM )
    ret += "Progrma.";
  if( status & FLT_HARDWARE )
    ret += "Hardware.";
  if( status & FLT_MEM )
    ret += "Memória.";
  if( status & FLT_INIT )
    ret += "Inicialização.";
  if( status & FLT_MOTION )
    ret += "Movimento.";
  if( status & FLT_RANGE )
    ret += "Limites.";
  if( status & FLT_PSTAGE_FORCED_OFF )
    ret += "Desligamento forçado.";
  return ret;
}

std::string statusString( uint16_t status ) {
  std::string ret;
  if( status & STAT_POWER_ON )
    ret += "Ligado ";
  if( status & STAT_TARGET_REACHED )
    ret += "Alvo alcançado ";
  if( status & STAT_FERROR_RECOVERY )
    ret += "Recuperação de erro de seguimento ";
  if( status & STAT_RUN )
    ret += "Executando ";
  if( status & STAT_ENABLED )
    ret += "Habilitado ";
  if( status & STAT_FAULTSTOP )
    ret += "Parada por falha ";
  if( status & STAT_FERROR_WARNING )
    ret += "Alerta de erro de seguimento";
  if( status & STAT_USER_BIT )
    ret += "Usuário ";
  if( status & STAT_SERVO_READY )
    ret += "Pronto ";
  return ret;
}
