#include <granite/simplemotion_private.h>
#include <granite/vsd_cmd.h>
#include <granite_spi_interface.h>

//-----------------------------------------------------------------------------
int GraniteSPI::parameterIndex( std::string paramname ) {
  bool done=smfalse;
  int i=0;
  while(done==smfalse) {
    if( paramname == smParams[i].paramName ) {
      return i;
    }

    i++;
    if(smParams[i].paramNum==0)
      done = true; //last in table
  }

  return -1;
}

//-----------------------------------------------------------------------------
uint64_t GraniteSPI::graniteSetParam( const char *paramname, uint32_t value ) {
  uint32_t cmd1, cmd2;
  uint16_t paramnum = 0;

  int i = 0;
  uint32_t scaledvalue;

  //look in param table
  i = parameterIndex(paramname);
  if( i<0 ) return SM_ERR_PARAMETER; //not found

  paramnum = smParams[i].paramNum;
  scaledvalue = value * smParams[i].scalerMultiplier / smParams[i].scalerDivider; //scale

  cmd1 = graniteDriveCmd( CMD_UPLOAD_PARAM, scaledvalue );
  cmd2 = graniteDriveCmd( CMD_SET_PARAM, paramnum );

  return (cmd1 << 32) | cmd2;
}

//-----------------------------------------------------------------------------
//calculate CRC8
//-----------------------------------------------------------------------------
#define CRC_INIT 51
uint8_t GraniteSPI::calcCrc8(uint8_t cmd, uint16_t data ) {
  uint8_t crc;
  uint8_t testbyte;

  testbyte=(byte)cmd;
  crc=CRC8table[CRC_INIT^testbyte];

  testbyte=(byte)data;
  crc=CRC8table[(crc^testbyte)];

  testbyte=(byte)(data>>8);
  crc=CRC8table[(crc^testbyte)];

  return crc;
}

//-----------------------------------------------------------------------------
uint32_t GraniteSPI::graniteDriveCmd( uint16_t cmd, uint16_t data ) {
  uint8_t cksum;

  //calculate CRC8
  cksum = calcCrc8( cmd, data );

  uint16_t cmd1, cmd2;
  cmd1 = cksum+((data&0xff)<<8);
  cmd2 = (data>>8) + (cmd<<8);

  return (cmd1 << 16) | cmd2;
}

//-----------------------------------------------------------------------------
uint64_t GraniteSPI::graniteAbsTarget( uint32_t param ) {
  uint32_t cmd1, cmd2;
  cmd1 = graniteDriveCmd( CMD_UPLOAD_PARAM, param&0xffff );
  cmd2 = graniteDriveCmd( CMD_ABS32_TARGET_VALUE, param>>16 );
  return (cmd1 << 32) | cmd2;
}

//-----------------------------------------------------------------------------
uint32_t GraniteSPI::startHoming() {
  return graniteDriveCmd( CMD_HOMING, 1 ); // 1 = start homing
}

//-----------------------------------------------------------------------------
uint32_t GraniteSPI::clearFaults() {
  return graniteDriveCmd( CMD_CLR_FAULTBITS, 0xffff );
}

//-----------------------------------------------------------------------------
