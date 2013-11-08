#include <rhombus.h>
#include <units.h>

//-----------------------------------------------------------------------------
Rhombus::Rhombus( int32_t a, int32_t l, uint8_t n, double wspeed ) : MatrixTrajectory(l/2) {
  int32_t xbase = 0,
          xstep = l / (4 * n);
  for(uint8_t i = 0; i < n; ++i) {
    add( Coordinate( xbase +   xstep,  a/2. ) );
    add( Coordinate( xbase + 3*xstep, -a/2. ) );
    add( Coordinate( xbase + 4*xstep,     0 ) );
    xbase += 4*xstep;
  }

  for(uint8_t i = 0; i < n; ++i) {
    int sig = i%2 == 0 ? 1 : -1;
    add( Coordinate( xbase -   xstep, sig*a/2. ) );
    add( Coordinate( xbase - 2*xstep,        0 ) );
    xbase -= 2*xstep;
  }

  double xspeed = 3 * wspeed,
         yspeed = 6 * n * a * wspeed / l;
  torch_speed_ = sqrt( xspeed*xspeed + yspeed*yspeed );
  amplitude_   = a;
  yalternate_  = true;
}
//-----------------------------------------------------------------------------
