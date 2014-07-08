#include <triangular.h>

//-----------------------------------------------------------------------------
TriangularTrajectory::TriangularTrajectory(double spd, double lmbd, double ampl,
                     double sstop, double istop, double f, const Vector3D &rotate_vec, double deg_xang ) : AbstractTrajectory(rotate_vec, deg_xang) {
  // domain
  if( sstop < 0 || sstop > 1 ) sstop = 0;
  if( istop < 0 || istop > 1 ) istop = 0;
  if( f < 0 || f > 4 ) f = 0;

  double lbdmm = fabs(lmbd) / TO_PULSES,
         vsmm = fabs(spd) / TO_RPM,
         dsup = lbdmm * sstop,
         dinf = lbdmm * istop,
         dint = lbdmm - (dsup+dinf);
         vext = f * vsmm,
         text = 0;
  if( vext > 1e-5 )
    text = (dsup + dinf) / vext;

  double amplmm = ampl/TO_PULSES,
         vint = sqrt(4*amplmm*amplmm+dint*dint) * vsmm / (ldbmm - vsmm * text);

  int period_count = 0.5 + rotate_vec.length()/lmbd;
  addRepeatable( period_count, lmbd, dsup*TO_PULSES, dinf*TO_PULSES, ampl, vint, vext );
  rotate();
}

//-----------------------------------------------------------------------------
void TriangularTrajectory::applyCorrection(double spd, double lmbd, double ampl,
                     uint32_t sstop, uint32_t istop) {
    boost::lock_guard<boost::mutex> lock(data_mutex_);
    if( index_ > positions_.size() - 2) return;
    printf("Correction: spd: %f lmbd: %f amp: %f sstop: %u istop: %u\n",
             spd, lmbd, ampl, sstop, istop );

    double xspeedmm  = double(spd) / TO_RPM,
           period    = lmbd / (xspeedmm * TO_PULSES),
           sstoplen  = xspeedmm * sstop * TO_PULSES / 1000.,
           istoplen  = xspeedmm * istop * TO_PULSES / 1000.,
           risexlen  = (lmbd - sstoplen - istoplen)/4.,
           yspeedmm  = 2. * (ampl/TO_PULSES)  / (period - (sstop+istop)/1000.),
           risespd   = Vector2D(xspeedmm, yspeedmm).length();

    uint32_t idx = index_-1;
    Vector3D cur = unrotate( positions_[idx] ),
             nxt = unrotate( positions_[idx+1] );
    if( fabs(nxt.y() - cur.y()) < 1e-5 ) { ++idx; cur = nxt; }

    eraseFrom( idx+1 );

    if( cur.y() > 0 ) {
      addA( Vector3D( cur.x() + 2*risexlen, -ampl/2., 0), risespd );
      if( istop ) addR( Vector3D( istoplen, 0, 0 ), xspeedmm );
      addR( Vector3D( 2*risexlen,  ampl, 0 ), risespd  );
    } else {
      addA( Vector3D( cur.x() + 2*risexlen,  ampl/2., 0), risespd );
    }

    double total_length = rotation_vec_.length() - accumulator_.x() + risexlen;
    int period_count = 0.5 + total_length/lmbd;
    addRepeatable( period_count - 1, sstop, istop, risexlen, ampl, risespd, xspeedmm );

    for( int i = idx + 1; i < positions_.size(); ++i )
      positions_[i] = rotate(positions_[i]);
}

//-----------------------------------------------------------------------------
void TriangularTrajectory::addRepeatable( int count, double lmbd, double dsup, double dinf,
                                          double ampl, double vint, double vext ) {
  double xstep = (lmbd - dsup - dinf) / 4;
  for( int i = 0; i < count; ++i ) {
    addR( Vector3D( xstep, ampl/2, 0 ), vint  );
    if( dsup > 1e-5 ) addR( Vector3D( dsup,       0, 0 ), vext );
    addR( Vector3D( 2*xstep, -ampl, 0 ), vint  );
    if( dinf > 1e-5 ) addR( Vector3D( dinf,       0, 0 ), vext );
    addR( Vector3D( xstep, ampl/2, 0 ), vint  );
  }
}

//-----------------------------------------------------------------------------
void TriangularTrajectory::draft( PositionVector &out, double spd, double l, double ampl, double sstop, double istop, double f ) {
  TriangularTrajectory e( spd, l, ampl, sstop, istop, f, Vector3D(4*l,0,0), 0);
  out.clear();
  out.push_back( e.initialOffset() );
  out.insert( out.end(), e.positions_.begin(), e.positions_.end() );
}

//-----------------------------------------------------------------------------
