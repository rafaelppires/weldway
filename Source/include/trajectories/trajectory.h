#ifndef _TRAJECTORY_H_
#define _TRAJECTORY_H_

#include <vectorxd.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <units.h>
#include <boost/thread.hpp>
#include <trajectory_transform.h>

//-----------------------------------------------------------------------------
typedef std::vector<Vector3D>     PositionVector;
typedef std::vector<Vector2D>     TorchVector;
typedef std::vector<double>       SpeedVector;
typedef std::pair<double,uint8_t> SincPair; /* percent of time; value */
typedef std::vector<SincPair>     SincVector;

//-----------------------------------------------------------------------------
class AbstractTrajectory {
public:
  AbstractTrajectory( TrajectoryTransformPtr tt );

  virtual const PositionVector& positions() const { return positions_; }
  virtual const SpeedVector& speeds() const { return speeds_; }
  virtual Vector3I initialOffset() const { return Vector3I(); }
  virtual bool getPoint(Vector3D &pos, double &spd, Vector2D &torch, SincPair &s, double &progress );
  virtual bool controlsTorch() { return transform_->controlsTorch(); }

protected:
  void setReference( const Vector3D & ); // set last point as reference for future relative adds
  void setReference(); // set last point as reference for future relative adds
  void addR( const Vector3D &delta, double spdmm, const SincPair &s = SincPair(-1,0) ); // Relative
  void addA( const Vector3D &pos, double spdmm, const SincPair &s = SincPair(-1,0)  );   // Absolute
  void eraseFrom( uint32_t idx );
  void rotate();

  Vector3D accumulator_;
  PositionVector positions_;
  SpeedVector speeds_;
  TorchVector torch_pos_;
  SincVector sinc_;
  uint32_t index_;
  boost::mutex data_mutex_;
  TrajectoryTransformPtr transform_;
  bool controls_torch_;
};

//-----------------------------------------------------------------------------
typedef boost::shared_ptr< AbstractTrajectory > AbsTrajectoryPtr;

//-----------------------------------------------------------------------------

#endif
