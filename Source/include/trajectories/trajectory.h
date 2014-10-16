#ifndef _TRAJECTORY_H_
#define _TRAJECTORY_H_

#include <vectorxd.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <units.h>
#include <boost/thread.hpp>
#include <trajectory_transform.h>

//-----------------------------------------------------------------------------
typedef std::vector<Vector3D>  PositionVector;
typedef std::vector<double>    SpeedVector;

//-----------------------------------------------------------------------------
class AbstractTrajectory {
public:
  AbstractTrajectory( TrajectoryTransformPtr tt );

  virtual const PositionVector& positions() const { return positions_; }
  virtual const SpeedVector& speeds() const { return speeds_; }
  virtual Vector3I initialOffset() const { return Vector3I(); }
  virtual bool getPoint( Vector3D &pos, double &spd, double &progress );

private:
  void setupMatrixes( double xangle );

protected:
  void setReference( const Vector3D & ); // set last point as reference for future relative adds
  void setReference(); // set last point as reference for future relative adds
  void addR( const Vector3D &delta, double spdmm ); // Relative
  void addA( const Vector3D &pos, double spdmm );   // Absolute
  void eraseFrom( uint32_t idx );
  void rotate();
  Vector3D rotate( const Vector3D &vec ) const;
  Vector3D unrotate( const Vector3D &vec ) const;

  MatrixLD rotation_matrix_, unrotation_matrix_;
  Vector3D accumulator_, rotation_vec_;
  PositionVector positions_;
  SpeedVector speeds_;
  uint32_t index_;
  boost::mutex data_mutex_;
  TrajectoryTransformPtr transform_;
};

//-----------------------------------------------------------------------------
typedef boost::shared_ptr< AbstractTrajectory > AbsTrajectoryPtr;

//-----------------------------------------------------------------------------

#endif
