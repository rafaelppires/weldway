#ifndef _UNITS_H_
#define _UNITS_H_

#include <string>
#include <boost/shared_ptr.hpp>

#define RT  100 // revolutions per meter
#define PPR 400 // pulses per revolution
#define SPEED_RANGE_FROM   1 // mm/s
#define SPEED_RANGE_TO   100 // mm/s
#define SPEED_RANGE      (SPEED_RANGE_TO-SPEED_RANGE_FROM)

// Default distance unit is milimeters
// Default time unit is seconds
#define STEP_LEN  (1000./(RT*PPR))
#define TO_M      (1./1000)
#define TO_CM     (1./10)
#define TO_IN     (1./25.4)
#define TO_PULSES (1./STEP_LEN)
#define TO_M_MIN  (60./1000)
#define TO_RPM    (60.*RT/1000)
#define TO_RPS    (RT/1000.)

class UnitConv {
public:
  UnitConv( double f, double t );
  virtual double getConv( std::string unit ) { return 1; }
  double from, to, range;
};

typedef boost::shared_ptr<UnitConv> UnitConvPtr;

class SpeedConv : public UnitConv {
public:
  SpeedConv( double a, double b ) : UnitConv(a,b) {}
  virtual double getConv( std::string unit );
};

class PositionConv : public UnitConv {
public:
  PositionConv( double a, double b ) : UnitConv(a,b) {}
  virtual double getConv( std::string unit );
};

#endif
