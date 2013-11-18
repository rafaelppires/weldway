#ifndef _MATRIX_TRAJECTORY_H_
#define _MATRIX_TRAJECTORY_H_

#include <trajectory.h>
#include <stdint.h>
#include <vector>
#include <vector2d.h>

class MatrixTrajectory : public AbstractTrajectory {
public:
  typedef Vector2D Coordinate;
  MatrixTrajectory(uint32_t xsteplen);

  virtual bool finished();
  virtual AbstractProtocol::ConcurrentCmmd32 speed();
  virtual AbstractProtocol::ConcurrentCmmd32 position();
  virtual boost::chrono::milliseconds interval();
  
protected:
  uint32_t index() { return step_ % trajectory_.size(); }
  void add( const Coordinate& );
  double xsteplen_, torch_speed_, amplitude_;
  bool yalternate_;

private:
  typedef std::vector< Coordinate > TrajectoryPoints;
  TrajectoryPoints trajectory_;
  uint32_t step_, interval_;
  double xposbase_;
  Coordinate last_, current_;
  bool moveto_done_;
  int ysignal_;
};

#endif
