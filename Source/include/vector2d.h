#ifndef _VECTOR2D_H_
#define _VECTOR2D_H_

#include <cmath>

template< unsigned int N, typename T >
class Vector {
public:

  template <typename T2>
  long double distance( const Vector<N,T2> &v2 ) {
    long double ret = 0;
    for(unsigned int i = 0; i < N; ++i) {
      long double d = v[i] - v2(i);
      ret += d*d;
    }
    return sqrt( ret );
  }

  template <typename T2>
  Vector<N,T> operator-( const Vector<N,T2> &v2 ) {
    Vector<N,T> ret;
    for(unsigned int i = 0; i < N; ++i )
      ret(i) = v[i] - v2(i);
    return ret;
  }

  Vector<N,T> operator*( T k ) {
    Vector<N,T> ret;
    for(unsigned int i = 0; i < N; ++i )
    ret(i) = v[i] * k;
    return ret;
  }

  const T& operator()( unsigned int i ) const {
    return v[i>=N ? N-1 : i];
  }

  T& operator()( unsigned int i ) {
    return v[i>=N ? N-1 : i];
  }

  T v[N];
};

template< typename T>
class Vector2 : public Vector<2, T> {
  typedef Vector<2, T> s;
public:
  Vector2( const s& v ) { x() = v(0); y() = v(1); }
  Vector2( T a, T b ) { x() = a; y() = b; }
  T& x() { return s::v[0]; }
  T& y() { return s::v[1];}
};

typedef Vector2<double> Vector2D;

#endif
