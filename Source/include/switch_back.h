#ifndef _SWITCHBACK_H_
#define _SWITCHBACK_H_

#include <trajectory.h>

class SwitchBackTrajectory : public AbstractTrajectory {
public:
  SwitchBackTrajectory( int32_t fwl, int32_t fws, int32_t bwl, int32_t bws ) :
    fwlen_(fwl), fwspd_(fws), bwlen_(fws), bwspd_(bwl) {}

private:
  int32_t fwlen_, fwspd_, bwlen_, bwspd_;
};

#endif
