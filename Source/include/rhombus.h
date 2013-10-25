#ifndef _RHOMBUS_H_
#define _RHOMBUS_H_

#include <trajectory.h>
#include <vector2d.h>

class Rhombus : public AbstractTrajectory {
public:
  Rhombus(int32_t a, int32_t l, uint8_t n, double wspeed);
  virtual bool finished();
  virtual AbstractProtocol::ConcurrentCmmd32 speed();
  virtual AbstractProtocol::ConcurrentCmmd32 position();
  virtual boost::chrono::milliseconds interval();

private:
  uint8_t count_, cycle_;
  uint32_t step_;
  int32_t xspeed_, yspeed_, amplitude_, xbase_, length_;
  bool finished_;
  Vector2D current_, last_;
};

#endif
