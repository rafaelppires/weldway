#ifndef _E_TRAJECTORY_H_
#define _E_TRAJECTORY_H_

#include <trajectory.h>
#include <stdint.h>

class ETrajectory : public AbstractTrajectory {
public:
  ETrajectory(double spd, double l, double ampl, double rho,  const Vector3D &rotate_vec, double deg_xang );
  Vector3I initialOffset() const;
  void applyCorrection(double spd, double l, double ampl , double rho);
  void addRepeatable( uint16_t count );
  void getSingle( PositionVector&p, SpeedVector&s );

  static void draft(PositionVector &out, double spd, double l, double ampl , double rho);
private:
  void defineSingle( double spd, double l );
  Vector3D f( double arc );
  double l_, rho_, A_;
  PositionVector psingle_;
  SpeedVector ssingle_;
  Vector3D offset_;
};

#endif
