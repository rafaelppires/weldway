#ifndef _SWITCHBACK_H_
#define _SWITCHBACK_H_

#include <trajectory.h>
#include <units.h>

class SwitchBackTrajectory : public AbstractTrajectory {
public:
  SwitchBackTrajectory( int32_t fwl, int32_t fws, int32_t bwl, int32_t bws, PositionConv pc, SpeedConv sc );

  virtual bool finished();
  virtual uint16_t speed();
  virtual AbstractProtocol::ConcurrentCmmd position();
  virtual boost::chrono::milliseconds interval();

private:
  int32_t fwlen_, fwspd_, bwlen_, bwspd_;
  bool finished_;
  uint32_t step_;
  double fwinterval_, bwinterval_;
  int16_t last_;
};

#endif
