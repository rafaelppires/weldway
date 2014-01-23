#ifndef _TRAJECTORY_H_
#define _TRAJECTORY_H_

#include <vectorxd.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <units.h>

//-----------------------------------------------------------------------------
typedef std::vector<Vector3D>  PositionVector;
typedef std::vector<double>    SpeedVector;

//-----------------------------------------------------------------------------
class AbstractTrajectory {
public:
  AbstractTrajectory() {}

  virtual const PositionVector& positions() const { return positions_; }
  virtual const SpeedVector& speeds() const { return speeds_; }

protected:
  void add( const Vector3D &delta, double spdmm ) {
    accumulator_ += delta;
    positions_.push_back( accumulator_ );
    speeds_.push_back( spdmm * TO_RPM );
  }

  Vector3D accumulator_;
  PositionVector positions_;
  SpeedVector speeds_;
};

//-----------------------------------------------------------------------------
typedef boost::shared_ptr< AbstractTrajectory > AbsTrajectoryPtr;

//-----------------------------------------------------------------------------

#endif
