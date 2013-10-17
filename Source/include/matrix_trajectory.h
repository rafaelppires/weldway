#ifndef _MATRIX_TRAJECTORY_H_
#define _MATRIX_TRAJECTORY_H_

#include <trajectory.h>
#include <stdint.h>
#include <vector>

class MatrixTrajectory : public AbstractTrajectory {
public:
  typedef std::pair<double,double> Coordinate;
  MatrixTrajectory();

  virtual bool finished();
  virtual AbstractProtocol::ConcurrentCmmd32 speed();
  virtual AbstractProtocol::ConcurrentCmmd32 position();
  virtual boost::chrono::milliseconds interval();
  
protected:
  void add( const Coordinate& );
  
private:
  typedef std::vector< Coordinate > TrajectoryPoints;
  TrajectoryPoints trajectory_;
};

#endif
