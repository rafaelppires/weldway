#include <windows.h>
#include <setupapi.h>
#include <parallel_port.h>
#include <cfgmgr32.h>
#include <boost/algorithm/string.hpp>

//-----------------------------------------------------------------------------
ParallelPort::ParallelPort() : index_(-1) {
  //devlist_ = list();
}

//-----------------------------------------------------------------------------
std::string ParallelPort::toStdString( char *buf ) {
  std::string ret( buf );
  if( UNICODE ) {
    std::wstring ws((wchar_t*)buf);
    ret = std::string(ws.begin(),ws.end());
  }
  return ret;
}

//-----------------------------------------------------------------------------
// Checks with windows the available parallel ports and their addresses
//-----------------------------------------------------------------------------
ParallelPort::ParallelList ParallelPort::list() {
  ParallelList ret;

  HDEVINFO hdev_info = SetupDiGetClassDevs( 0L, 0L, 0L,
                           DIGCF_PRESENT | DIGCF_ALLCLASSES | DIGCF_PROFILE ) ;  
  if( hdev_info == (void*)-1 ) {
    return ret;
  }
  
  uint32_t idx = 0;
  SP_DEVINFO_DATA spdev_info = {0};  
  spdev_info.cbSize = sizeof(SP_DEVINFO_DATA);  
  char szBuf[ 2048 ]; szBuf[0] = 0; DWORD rs;
  while( SetupDiEnumDeviceInfo( hdev_info, idx++, &spdev_info ) ) {
    bool ok = SetupDiGetDeviceRegistryProperty( hdev_info, &spdev_info, SPDRP_CLASS, 0L,
                                                         (PBYTE)szBuf, 2048, &rs);
    if( toStdString(szBuf) != "Ports" )
      continue;

    if( !ok )
      continue;

    SetupDiGetClassDescription(&spdev_info.ClassGuid, (PWSTR)szBuf, 2048, 0);
    SetupDiGetDeviceRegistryProperty( hdev_info, &spdev_info, SPDRP_FRIENDLYNAME, 0L, (PBYTE)szBuf, 2048, 0);

    std::string friendly_name = toStdString( szBuf );
    SetupDiGetDeviceRegistryProperty( hdev_info, &spdev_info, SPDRP_SERVICE, 0L, (PBYTE)szBuf, 2048, 0);

    if( !boost::iequals(toStdString( szBuf ), "parport") )
      continue;

    LOG_CONF  firstLogConf, nextLogConf, rdPrevResDes;
    CONFIGRET cmdRet;
    // ALLOC_LOG_CONF: A resource list identifying resources currently in use
    // by a device instance. Only one allocated configuration can exist for 
    // each device instance.
    cmdRet = CM_Get_First_Log_Conf( &firstLogConf, spdev_info.DevInst, ALLOC_LOG_CONF);
    if( cmdRet != CR_SUCCESS )
      continue;

    cmdRet = CM_Get_Next_Res_Des(&nextLogConf, firstLogConf, ResType_IO, 0L, 0L );
    if( cmdRet != CR_SUCCESS ) {
      CM_Free_Res_Des_Handle( firstLogConf );
      continue;
    }

    while( true ) {
      ULONG ulSize = 0;
      cmdRet = CM_Get_Res_Des_Data_Size(&ulSize, nextLogConf, 0L);
      if( cmdRet != CR_SUCCESS )
        break;

      cmdRet = CM_Get_Res_Des_Data(nextLogConf, szBuf, std::min<ULONG>(ulSize,sizeof(szBuf)), 0L);
      if( cmdRet != CR_SUCCESS )
        break;

      IO_DES *io = (IO_DES*)szBuf;

      ret.push_back( std::make_pair( friendly_name, uint32_t(io->IOD_Alloc_Base) ) );

      rdPrevResDes = 0;
      cmdRet = CM_Get_Next_Res_Des(&rdPrevResDes, nextLogConf, ResType_IO, 0L, 0L);
      
      if (cmdRet != CR_SUCCESS)
         break;
      else {
        CM_Free_Res_Des_Handle(nextLogConf);
        nextLogConf = rdPrevResDes;
      };
    }
    CM_Free_Res_Des_Handle( nextLogConf );
    CM_Free_Res_Des_Handle( firstLogConf );
  }

  return ret;
}

//-----------------------------------------------------------------------------
void ParallelPort::select( int32_t idx ) {
  index_ = idx >= 0 && idx < int(devlist_.size()) ? idx : -1;
}
