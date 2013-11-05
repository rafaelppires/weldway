#include <rhombus.h>
#include <units.h>

//-----------------------------------------------------------------------------
Rhombus::Rhombus( int32_t a, int32_t l, uint8_t n, double wspeed ) : {
  int32_t xbase = 0;
  for(uint8_t i = 0; i < n; ++i) {
    add( Coordinate( xbase +   xstep,  ampl ) );
    add( Coordinate( xbase + 3*xstep, -ampl ) );
    add( Coordinate( xbase + 4*xstep,     0 ) );
    xbase += 4*xstep;
  }

  for(uint8_t i = 0; i < n; ++i) {
    int sig = n%2 == 0 ? 1 : -1;
    add( Coordinate( xbase -   xstep, sig*ampl ) );
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

/*
//-----------------------------------------------------------------------------
Rhombus::Rhombus( int32_t a, int32_t l, uint8_t n, double wspeed ) :
    amplitude_(a), length_(l), finished_(false), step_(0), current_(0,0), last_(0,0), count_(n), cycle_(0) {
  xspeed_ = 3 * wspeed;
  yspeed_ = 6 * n * a * wspeed / l;
  printf("Xspd: %d Yspd: %d\n", xspeed_, yspeed_ );
}

//-----------------------------------------------------------------------------
bool Rhombus::finished() {
  return finished_;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 Rhombus::speed() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  if( !step_ ) {
    ret[ X_AXIS ] = ret[ Y_AXIS ] = 650;
  } else if(step_ == 1) {
    ret[ X_AXIS ] = xspeed_;
    ret[ Y_AXIS ] = yspeed_;
  }
  return ret;
}

//-----------------------------------------------------------------------------
AbstractProtocol::ConcurrentCmmd32 Rhombus::position() {
  AbstractProtocol::ConcurrentCmmd32 ret;
  int32_t yoffset = 400; // 10 mm
  uint8_t cstep;
  bool done = false;
  while( !done ) {
  cstep = step_ % (6 * count_);
  if( !step_ ) {
    ret[ X_AXIS ] = 4000;
    ret[ Y_AXIS ] = - yoffset - amplitude_ / 2;
    xbase_ = 4000;
    done = true;
  } else if( cstep == 0 || cstep == 4 * count_ ) {
    if( cstep == 0 ) { ++cycle_; xbase_ += length_/2; }
    ret[ X_AXIS ] = cstep == 0 ? (xbase_+length_) : (xbase_+length_/2);
    ret[ Y_AXIS ] = - yoffset - amplitude_ / 2;
    done = true;
  } else if(  cstep % 4 == 1 ) {
    ret[ Y_AXIS ] = - yoffset - ( cycle_%2==0 ? amplitude_ : 0 );
    done = true;
  } else if( cstep % 4 == 3 ) {
    ret[ Y_AXIS ] = - yoffset - ( cycle_%2==0 ? 0 : amplitude_ );
    done = true;
  }
  ++step_;
  }

  --step_;

  if( ret.find(X_AXIS) != ret.end() )
      current_.x() = ret[ X_AXIS ];
  current_.y() = ret[ Y_AXIS ];
  if( cycle_ == 20 ) finished_ = true;
  return ret;
}

//-----------------------------------------------------------------------------
boost::chrono::milliseconds Rhombus::interval() {
  int ret = 1000. * TO_RPM * abs(current_.y() - last_.y()) / (yspeed_ * TO_PULSES);
  if( !step_ ) ret = 3000;
  ++step_;
  last_ = current_;
  return boost::chrono::milliseconds(ret);
}

//-----------------------------------------------------------------------------
*/
