#ifndef _TRIANGULAR_H_
#define _TRIANGULAR_H_

#include <trajectory.h>
#include <stdint.h>

class TriangularTrajectory : public AbstractTrajectory {
public:
  TriangularTrajectory(int32_t spd, double freq, int32_t ampl,
                       uint32_t sstop, uint32_t istop, double total_length){
    double period = 1. / freq,
        xspeedmm  = double(spd) / TO_RPM,
        lambda    = xspeedmm * period * TO_PULSES,
        sstoplen  = xspeedmm * sstop * TO_PULSES / 1000.,
        istoplen  = xspeedmm * istop * TO_PULSES / 1000.,
        risexlen  = (lambda - sstoplen - istoplen)/4.,
        yspeedmm  = 2. * (ampl/TO_PULSES)  / (period - (sstop+istop)/1000.),
        risespd   = Vector2D(xspeedmm, yspeedmm).length();
     int period_count = 0.5 + total_length/lambda;


     add( Vector3D( risexlen, ampl/2., 0 ), risespd );
     for( int i = 0; i < period_count - 1; ++i ) {
       if( sstop ) add( Vector3D( sstoplen,       0, 0 ), xspeedmm );
       add( Vector3D( 2*risexlen, -ampl, 0 ), risespd  );
       if( istop ) add( Vector3D( istoplen,       0, 0 ), xspeedmm );
       add( Vector3D( 2*risexlen,  ampl, 0 ), risespd  );
     }
     if( sstop ) add( Vector3D( sstoplen,          0, 0 ), xspeedmm );
     add( Vector3D( 2*risexlen,    -ampl, 0 ), risespd  );
     if( istop ) add( Vector3D( istoplen,          0, 0 ), xspeedmm );
     add( Vector3D( risexlen,    ampl/2., 0 ), risespd  );

     printf("TriangularTrajectory spd: %d freq: %f, A: %d, ts: %d ti: %d TL: %f\n",
             spd, freq, ampl, sstop, istop, total_length );
     printf("Lambda: %f s(%f) i(%f) Iter count: %d (%f)\n", lambda, sstoplen, istoplen, period_count, total_length/lambda  );
  }
};

#endif
