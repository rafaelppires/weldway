#ifndef _TRAJECTORY_H_
#define _TRAJECTORY_H_

#include <vectorxd.h>
#include <vector>
#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------
typedef std::vector<Vector3I>  PositionVector;
typedef std::vector<Vector3US> SpeedVector;

//-----------------------------------------------------------------------------
class AbstractTrajectory {
public:
  AbstractTrajectory() {}

  virtual const PositionVector& positions() const { return positions_; }
  virtual const SpeedVector& speeds() const { return speeds_; }

protected:
  PositionVector positions_;
  SpeedVector speeds_;
};

//-----------------------------------------------------------------------------
typedef boost::shared_ptr< AbstractTrajectory > AbsTrajectoryPtr;

//-----------------------------------------------------------------------------

#endif
