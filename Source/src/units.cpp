#include <units.h>

//-----------------------------------------------------------------------------
UnitConv::UnitConv( double f, double t ) : from(f), to(t), range(t-f) {}

//-----------------------------------------------------------------------------

double SpeedConv::getConv( std::string unit ) {
  double ret = 1;
  if( unit == "m/min" )
    ret = TO_M_MIN;
  else if( unit == "rpm" )
    ret = TO_RPM;
  else if( unit == "rps" )
    ret = TO_RPS;
  return ret;
}

//-----------------------------------------------------------------------------
double PositionConv::getConv( std::string unit ) {
  double ret = 1;
  if( unit == "m" )
    ret = TO_M;
  else if( unit == "cm" )
    ret = TO_CM;
  else if( unit == "\"" )
    ret = TO_IN;
  else if( unit == "pulsos" )
    ret = TO_PULSES;
  return ret;
}
