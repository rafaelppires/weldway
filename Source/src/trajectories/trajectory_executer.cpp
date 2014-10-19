#include <trajectory_executer.h>
#include <boost/chrono.hpp>
#include <Matrix.h>
#include <granite/vsd_cmd.h>
using boost::chrono::high_resolution_clock;
using boost::chrono::nanoseconds;
using boost::chrono::milliseconds;

//-----------------------------------------------------------------------------
TrajectoryExecuter::TrajectoryExecuter( AbsTrajectoryPtr t, boost::shared_ptr< AbstractProtocol > comm )
  : trajectory_(t), comm_(comm), finished_(false), offset_updated_(false), controls_torch_(false), last_torch_spd_(0,0), last_spd_(0,0,0) {
  acceleration_.x() = acceleration_.y() = acceleration_.z() = 4000 / (0.025 * TO_RPM); // mm/s²
}
//-----------------------------------------------------------------------------
void TrajectoryExecuter::operator()() {
  if( !comm_->homingDone() ) { cancel(); return; }

  Vector3D last_pos(0,0,0), delta(0,0,0);
  uint16_t interval;

  last_pos = gotoInitial();

  high_resolution_clock::time_point now, start;

  Vector3D cur_point;
  Vector2D torch;
  Vector2I cur_torch(0,0), delta_torch(0,0), last_torch(0,0);
  double cur_spd, progress;
  comm_->startTorch();
  waitFor( 2000 );
  controls_torch_ = trajectory_->controlsTorch();
  while( trajectory_->getPoint(cur_point, cur_spd, torch, progress) ) {
    cur_torch = comm_->angularPulsesOffset(ANGULAR_VERTICAL, torch.x()) +
                comm_->angularPulsesOffset(ANGULAR_HORIZONTAL, torch.y());

    start = high_resolution_clock::now();
    delta = cur_point - last_pos;
    delta_torch = cur_torch - last_torch;

    {
      boost::lock_guard<boost::mutex> lock(correction_mutex_);
      if( offset_updated_ ) {
        delta += offset_;
        offset_ = Vector3D();
        offset_updated_ = false;
      }
    }

    Vector2US spd_torch;
    Vector3US spds = getSpeedsAndInterval( delta, interval, cur_spd, delta_torch, spd_torch );
    deliverSpeedsAndPositions( delta, spds, delta_torch, spd_torch );


    fflush(stdout);
    now = high_resolution_clock::now();

    int diff = interval - boost::chrono::duration_cast<milliseconds>(now - start).count();
    if( diff > 0 )
      waitFor( diff );
    last_pos = cur_point;
    if( controls_torch_ ) last_torch = cur_torch;

    if( progress_callback_ ) progress_callback_( progress );
    if( finished() ) { break; }
  }
  cancel();
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::deliverSpeedsAndPositions( const Vector3I &delta, const Vector3US &speeds, const Vector2I &delta_torch, const Vector2US &spd_torch ) {
  AbstractProtocol::ConcurrentCmmd32 poscmmds, spdcmmds;
  uint16_t sx = speeds.x(), sy = speeds.y(), sz = speeds.z();
  int32_t  dx = delta.x(), dy = delta.y(), dz = delta.z();

  if( dx && sx && sx != last_spd_.x() ) spdcmmds[ X_AXIS ] = sx;
  if( dy && sy && sy != last_spd_.y() ) spdcmmds[ Y_AXIS ] = sy;
  if( dz && sz && sz != last_spd_.z() ) spdcmmds[ Z_AXIS ] = sz;
  if( controls_torch_ ) {
    uint16_t sa = spd_torch.x(), sb = spd_torch.y();
    if( delta_torch.x() && sa && sa != last_torch_spd_.x() ) spdcmmds[ A_AXIS ] = sa;
    if( delta_torch.y() && sb && sb != last_torch_spd_.y() ) spdcmmds[ B_AXIS ] = sb;
  }

  current_pos_ += delta;
  if( delta.x() ) poscmmds[ X_AXIS ] =  current_pos_.x();
  if( delta.y() ) poscmmds[ Y_AXIS ] = -current_pos_.y();
  if( delta.z() ) poscmmds[ Z_AXIS ] =  current_pos_.z();
  if( controls_torch_ ) {
    int32_t da = delta_torch.x(), db = delta_torch.y();
    current_torch_ += delta_torch;
    if( da ) poscmmds[ A_AXIS ] = current_torch_.x();
    if( db ) poscmmds[ B_AXIS ] = current_torch_.y();
  }

  comm_->sendSpdCmmds(spdcmmds);
  comm_->sendPosCmmds(poscmmds);

  last_spd_ = Vector3US(spdcmmds[ X_AXIS ], spdcmmds[ Y_AXIS ], spdcmmds[ Z_AXIS ]);
  if( controls_torch_ ) {
    last_torch_spd_ = Vector2US(spdcmmds[A_AXIS], spdcmmds[B_AXIS]);
  }
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::waitFor( uint32_t ms ) {
  static uint32_t min = 1000;
  if( ms < min ) std::cout << "Inter: " << (min=ms) << "\n";
  boost::this_thread::sleep_for( boost::chrono::milliseconds( ms ) );
}

//-----------------------------------------------------------------------------
Vector3D TrajectoryExecuter::gotoInitial() {
  Vector3D ret = trajectory_->initialOffset();
  uint16_t interv;
  trajectory_init_ += ret;
  Vector3I delta = trajectory_init_ - current_pos_;
  Vector2I dummy1(0,0); Vector2US dummy2(0,0);
  deliverSpeedsAndPositions( delta, getSpeedsAndInterval( delta, interv, 650, dummy1, dummy2 ), dummy1, dummy2 );
  int status = -1;
  while( status ) {
    status = 0;
    for( uint8_t i = 1; i < 8; i<<=1 )
      if( !(comm_->getStatus( StatusBits, i) & STAT_TARGET_REACHED) ) {
        status = -1;
        break;
      }
    if( status )
    boost::this_thread::sleep_for( boost::chrono::milliseconds( 100 ) );
  }
  return ret;
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::setAngularOffset( double angle ) {
  overx_angle_ = angle;
}

//-----------------------------------------------------------------------------
Vector3US TrajectoryExecuter::getSpeedsAndInterval(const Vector3D &delta, uint16_t &interval, double res_spd, const Vector2I &delta_torch, Vector2US &spd_torch ) {
  // Decompose to find the axis projection
  Vector3D vr( delta.unary() );
  vr *= res_spd / TO_RPM; // mm/s
  // Adjust Speed
  interval = 0.5 + 1000. * delta.length() * TO_RPM / (TO_PULSES * res_spd); // ms
  Vector3US ret;
  //                   mm/s         mm/s²              s
  ret.x() = fixSpeed( abs(vr.x()), acceleration_.x(), interval/1000. );
  ret.y() = fixSpeed( abs(vr.y()), acceleration_.y(), interval/1000. );
  ret.z() = fixSpeed( abs(vr.z()), acceleration_.z(), interval/1000. );

  if( controls_torch_ ) {
    spd_torch.x()  = (delta_torch.x()*25.) / (6.*interval); // rpm
    spd_torch.y()  = (delta_torch.y()*25.) / (6.*interval); // rpm
  }

  return ret;
}
//-----------------------------------------------------------------------------
uint16_t TrajectoryExecuter::fixSpeed( double v, double a, double t ) {
  if( a * t / v > 4. ) return 0.5 + adjustedSpeed(v,a,t) * TO_RPM;
  return 0;
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::setLimits(const Vector3I &init ) {
  trajectory_init_ = init;
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::setCurrent(const Vector3I &last, const Vector2I &last_torch) {
  current_pos_ = last;
  current_torch_ = last_torch;
}

//-----------------------------------------------------------------------------
bool TrajectoryExecuter::finished() {
   bool ret;
   {
     boost::lock_guard<boost::mutex> lock(finish_mutex_);
     ret = finished_;
   }
   return ret;
 }

//-----------------------------------------------------------------------------
void TrajectoryExecuter::cancel() {
  boost::lock_guard<boost::mutex> lock(finish_mutex_);
  comm_->stopTorch();
  finished_ = true;
  if( progress_callback_ ) progress_callback_( 1 );
}

//-----------------------------------------------------------------------------
void TrajectoryExecuter::addLinearOffset( const Vector3D &offset ) {
  boost::lock_guard<boost::mutex> lock(correction_mutex_);
  if( offset_updated_ )
    offset_ += offset;
  else
    offset_ = offset;
  accumulated_offset_ += offset;
  offset_updated_ = true;
}

//-----------------------------------------------------------------------------
