#ifndef _TRIANGULAR_H_
#define _TRIANGULAR_H_

#include <trajectory.h>
#include <stdint.h>

class TriangularTrajectory : public AbstractTrajectory {
public:
  TriangularTrajectory( int32_t, double, int32_t, uint32_t, uint32_t );

  virtual bool finished();
  virtual AbstractProtocol::ConcurrentCmmd32 speed();
  virtual AbstractProtocol::ConcurrentCmmd32 position();
  virtual boost::chrono::milliseconds interval();
  virtual bool torchOn();

private:
  int32_t weld_spd_, amplitude_, vy_, ta_, tb_;
  double interval_, total_time_;
  uint32_t step_, sstop_, istop_;
  bool torch_on_;
};

#endif
