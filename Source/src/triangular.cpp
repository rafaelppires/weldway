#include <triangular.h>
#include <units.h>
#include <math.h>

//-----------------------------------------------------------------------------
TriangularTrajectory::TriangularTrajectory( int32_t spd, double freq, int32_t ampl,
                                            uint32_t sstop, uint32_t istop, double total_length ) {
  double period    = 1. / freq,
         xspeedmm  = double(spd) / TO_RPM,
         lambda    = xspeedmm * period * TO_PULSES,
         sstoplen  = xspeedmm * sstop * TO_PULSES / 1000.,
         istoplen  = xspeedmm * istop * TO_PULSES / 1000.,
         risexlen  = (lambda - sstoplen - istoplen)/4.,
         yspeedmm  = 2. * (ampl/TO_PULSES)  / (period - (sstop+istop)/1000.),
         risespd   = Vector2D(xspeedmm, yspeedmm).length();
  int period_count = 0.5 + total_length/lambda;

  Vector3I cur_pos;
  add( cur_pos, Vector3I( risexlen, ampl/2., 0 ), risespd );
  for( int i = 0; i < period_count - 1; ++i ) {
    add( cur_pos, Vector3I( sstoplen,       0, 0 ), xspeedmm );
    add( cur_pos, Vector3I( 2*risexlen, -ampl, 0 ), risespd  );
    add( cur_pos, Vector3I( istoplen,       0, 0 ), xspeedmm );
    add( cur_pos, Vector3I( 2*risexlen,  ampl, 0 ), risespd  );
  }
  add( cur_pos, Vector3I( sstoplen,          0, 0 ), xspeedmm );
  add( cur_pos, Vector3I( 2*risexlen,    -ampl, 0 ), risespd  );
  add( cur_pos, Vector3I( istoplen,          0, 0 ), xspeedmm );
  add( cur_pos, Vector3I( risexlen,    ampl/2., 0 ), risespd  );

  printf("TriangularTrajectory spd: %d freq: %f, A: %d, ts: %d ti: %d TL: %f\n",
          spd, freq, ampl, sstop, istop, total_length );
  printf("Lambda: %f s(%f) i(%f) Iter count: %d (%f)\n", lambda, sstoplen, istoplen, period_count, total_length/lambda  );
}

//-----------------------------------------------------------------------------
void TriangularTrajectory::add( Vector3I &accumulator, const Vector3I &delta, double spdmm ) {
  accumulator += delta;
  //printf("(%d,%d,%d) d(%d,%d,%d)\n", accumulator.x(), accumulator.y(), accumulator.z(),
  //                                     delta.x(), delta.y(), delta.z());
  positions_.push_back( accumulator );
  speeds_.push_back( spdmm * TO_RPM );
}

//-----------------------------------------------------------------------------
