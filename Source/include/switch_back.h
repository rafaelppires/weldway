#ifndef _SWITCHBACK_H_
#define _SWITCHBACK_H_

#include <trajectory.h>
#include <units.h>

class SwitchBackTrajectory : public AbstractTrajectory {
public:
  SwitchBackTrajectory(int32_t, int32_t);

  virtual bool finished();
  virtual AbstractProtocol::ConcurrentCmmd32 speed();
  virtual AbstractProtocol::ConcurrentCmmd32 position();
  virtual boost::chrono::milliseconds interval();
  virtual bool torchOn();

private:
  int32_t fwlen_, fwspd_, bwlen_, bwspd_;
  bool finished_, torch_on_;
  uint32_t step_;
  double fwinterval_, bwinterval_;
  int16_t last_pos_, last_spd_;
};

#endif
