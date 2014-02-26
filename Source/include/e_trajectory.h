#ifndef _E_TRAJECTORY_H_
#define _E_TRAJECTORY_H_

#include <trajectory.h>
#include <stdint.h>

class ETrajectory : public AbstractTrajectory {
public:
  ETrajectory(int32_t spd, double l, int32_t ampl,  const Vector3D &rotate_vec, double deg_xang ) : AbstractTrajectory(rotate_vec, deg_xang)  {
    double total_length = rotate_vec.length();
    int period_count = 0.5 + total_length/l;
    double t = (TO_RPM*l) / (TO_PULSES*spd),    // s
           k = ampl / (l*sqrt(2.)),             // nondimensional
           yoff = k * l * sqrt(2.) / 2.,
           d = sqrt(l*l*(k*k+1.)) / 2.,
           vr = (4. * d + l) / (TO_PULSES*t);   // mm / s;

    offset_.x() = 0.5*l;
    offset_.y() = -yoff;

    addR( Vector3D(  0.5*l,  yoff, 0 ), vr );
    addR( Vector3D( -0.5*l,  yoff, 0 ), vr );
    addR( Vector3D( -0.5*l, -yoff, 0 ), vr );
    addRepeatable( period_count - 1, l, yoff, vr );
    printf("T: %f l: %f d: %f k: %f Ts: %f\n", t, l, d, k, vr );

    rotate();
  }

  Vector3I initialOffset() const {
    int sig = rotation_vec_.x() / fabs(rotation_vec_.x());
    return rotate( Vector3D(offset_.x()*sig, offset_.y(), offset_.z()) );
  }

  void applyCorrection( int32_t spd, double l, int32_t ampl ) {
      boost::lock_guard<boost::mutex> lock(data_mutex_);
      if( index_ < 1 || index_ > positions_.size() - 2 ) return;

      Vector3D diff, cur, prev;
      uint32_t idx = index_;

      prev = unrotate( positions_[idx-1] );
      do {
        cur = unrotate( positions_[idx++] );
        diff = cur - prev;
        if( fabs(diff.x())<1e-5 ) diff.x() = 0;
        if( fabs(diff.y())<1e-5 ) diff.y() = 0;
        prev = cur;
      } while( idx < positions_.size() && (diff.x() >= 0 || diff.y() >= 0) );

      positions_.erase( positions_.begin()+idx, positions_.end() );
      speeds_.erase( speeds_.begin()+idx, speeds_.end() );


      double t = (TO_RPM*l) / (TO_PULSES*spd),    // s
             k = ampl / (l*sqrt(2.)),             // nondimensional
             yoff = k * l * sqrt(2.) / 2.,
             d = sqrt(l*l*(k*k+1.)) / 2.,
             vr = (4. * d + l) / (TO_PULSES*t);   // mm / s;

      int period_count = 0.5 + (rotation_vec_.length() - cur.x())/l;
      addRepeatable( period_count, l, yoff, vr );
  }

  void addRepeatable( uint16_t count, double l, double ysteplen, double vr ) {
      for( uint32_t i = 0; i < count; ++i ) {
        addR( Vector3D(  0.5*l, -ysteplen, 0 ), vr );
        addR( Vector3D(      l,        0, 0 ), vr );
        addR( Vector3D(  0.5*l,  ysteplen, 0 ), vr );
        addR( Vector3D( -0.5*l,  ysteplen, 0 ), vr );
        addR( Vector3D( -0.5*l, -ysteplen, 0 ), vr );
      }
      addR( Vector3D(  0.5*l, -ysteplen, 0 ), vr );
  }

private:
  Vector3D offset_;
};

#endif
