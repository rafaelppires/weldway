#ifndef MatrixH
#define MatrixH

#include <string.h>
#include <iostream>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <boost/shared_array.hpp>
using namespace std;

template<typename T> inline T fabs(T x){ return x<0?-x:x; }

struct MatrixException : public std::logic_error {
  MatrixException( const string &msg ) : std::logic_error( msg ) {}
};

template<typename T>
class Matrix {
public:
  Matrix() : m_(0), n_(0) {}

  Matrix(unsigned int n) : m_(n), n_(n){
    data_.reset( new T[n*n] );
    for(int i=0; i<n; i++)
      for(int j=0; j<n; j++)
        (*this)(i,j) = (i==j)?1:0;
  }

  Matrix(unsigned int m, unsigned int n) :
    m_(m), n_(n) {
    data_.reset( new T[m*n] );
  }

  ~Matrix() {}
  
  T& operator() (unsigned int i, unsigned int j) const{
    return data_[index(i,j)];
  }

  Matrix cofat(unsigned int ii, unsigned int jj) const{
    int n = n_ - 1;
    int k = 0;
    Matrix<T> R(n,n);
    for(int i=0; i<m_; i++)
      for(int j=0; j<n_; j++)
        if(i!=ii && j!=jj)
          R.data_[k++] = (*this)(i,j);
        
    return R;
  }

  T det() const{
    if(m_ != n_){
      stringstream ss;
      ss << "Only square matrixes have determinant! lines=" << m_ << " cols=" << n_;
      throw MatrixException( ss.str() );
    }

    const Matrix<T> &m = (*this);
    if(n_ == 1) return m(0,0);
    if(n_ == 2) return m(0,0)*m(1,1)-m(0,1)*m(1,0);
    if(n_ == 3) return m(0,0)*m(1,1)*m(2,2)
          +m(0,1)*m(1,2)*m(2,0)
          +m(1,0)*m(2,1)*m(0,2)
          -m(0,2)*m(1,1)*m(2,0)
          -m(0,1)*m(1,0)*m(2,2)
          -m(0,0)*m(1,2)*m(2,1);

    T d=0,e;
    int i = 0;
    for(int j=0; j<n_; j++)
      d += ((i+j)%2==0?1:-1)*cofat(i,j).det()*m(i,j);
    return d;
  }

  Matrix operator!() const{
    if(m_ != n_){
      stringstream ss;
      ss << "Only square matrixes can be inverted! lines=" << m_ << " cols=" << n_;
      throw MatrixException( ss.str() );
    }
    
    T d = det();
    if( fabs(d) < 1e-10 ){
      throw MatrixException( "This matrix is uninvertible. Determinant is null" );
    }

#ifndef GAUSS_ELIMINATION
    Matrix<T> R(m_,m_);
    for(int i=0; i<m_; i++)
      for(int j=0; j<n_; j++){
        R(i,j) = ((i+j)%2==0?1:-1)*cofat(j,i).det()/d;
      }
    return R;
#else
    Matrix<T> i(m_);
    return Matrix<T>::gaussian( *this, i );
#endif
  }

  Matrix& operator=(const Matrix &M) {
    m_ = M.m_; n_ = M.n_;
    data_.reset( new T[n_*m_] );
    for(int i=0; i<m_*n_; i++) data_[i] = M.data_[i];
    return *this;
  }

  Matrix operator*(const Matrix &M) const{
    if(n_ != M.lines()){
      stringstream ss;
      ss << "Cannot multiply matrix A( #lines, " <<  n_ << " ) * B( " 
         << M.lines() << ", #cols ). ";
      throw MatrixException( ss.str() + "The first's number of columns must be equal to the second's number of lines." );
    }

    Matrix<T> R(m_,M.columns());
    for(int i=0; i<m_; i++)
      for(int j=0; j<M.n_; j++) {
        R(i,j) = 0;
        for(int k=0; k<n_; k++){
          R(i,j) += (*this)(i,k) * M(k,j);
        }
      }
    return R;
  }

  Matrix operator~() const{
    Matrix<T> R(n_,m_);
    for(int i=0; i<m_; i++)
      for(int j=0; j<n_; j++)
        R(j,i) = (*this)(i,j);
    return R;
  }

  Matrix operator|(const Matrix &M) const{
    if(m_ != M.m_){
      stringstream ss;
      ss << "You can only concatenate matrixes with the same number of lines. A( " << m_ << ", #cols ) | B( " << M.m_ << ", #cols )";
      throw MatrixException( ss.str() );
    }
    Matrix<T> R(m_,n_+M.n_);
    
    for(int i=0; i < m_; i++){
      for(int j=0; j<n_+M.n_; j++){
        if(j<n_) R(i,j) = (*this)(i,j);
        else     R(i,j) = M(i,j-n_);
      }
    }
    return R;
  }

  friend ostream& operator << (ostream& o, const Matrix &m){
    for(int i=0; i<m.m_; i++){
      for(int j=0; j<m.n_; j++){
        T x = m(i,j);
        if(fabs(x)<1e-13) x=0;
        o << x << "\t";
      }
      o << "\n";
    }
    return o;
  }

  unsigned int lines() const{
    return m_;
  }

  unsigned int columns() const{
    return n_;
  }

  void switch_lines(unsigned int i1, unsigned int i2){
    if(i1 == i2) return;
    T x;
    const Matrix<T> &m = (*this);
    for(int j=0; j<n_; j++){
      x = m(i1,j);
      m(i1,j) = m(i2,j);
      m(i2,j) = x;
    }
  }

  static Matrix<T> gaussian(const Matrix<T> &A, const Matrix<T> &b) {
    if(A.m_ != A.n_ || A.m_ != b.m_){
      stringstream ss;
      ss << "Gaussian Elimination on A( " << A.m_ << ", " << A.n_ << " ) over B( " << b.m_ << ", " << b.n_ 
         << " ) only if they have the same number of lines and A is a square matrix.";
      throw MatrixException( ss.str() );
    }

    Matrix<T> A_; A_ = A;
    Matrix<T> b_; b_ = b;

    for(int j=0; j<A_.n_; j++){
      int k = -1;
      T max;
      for(int i=j; i<A_.m_; i++){
        if(k == -1 || fabs(A_(i,j)) > max){
          max = fabs(A_(i,j));
          k = i;
        }
      }

      if(j != k){
        A_.switch_lines(j,k);
        b_.switch_lines(j,k);
      }

      for(int i=j+1; i<A_.m_; i++){
        T mult = -A_(i,j) / A_(j,j);
        for(k=j; k<A_.n_; k++){
          A_(i,k) = A_(i,k) + A_(j,k) * mult;
        }
        for(k=0; k<b_.n_; k++){
          b_(i,k) = b_(i,k) + b_(j,k) * mult;
        }
      }


      T jj = A_(j,j);
      for(int i=j; i<A_.n_; i++)
        A_(j,i) /= jj;
      for(int i=0; i<b_.n_; i++)
        b_(j,i) /= jj;

      k = -1;
    }

    for(int j=A_.n_-1; j>0; j--){
      for(int i=j-1; i>=0; i--){
        T mult = -A_(i,j);
        for(int k=j; k<A_.n_; k++)
          A_(i,k) += mult * A_(j,k);
        for(int k=0; k<b_.n_; k++)
          b_(i,k) += mult * b_(j,k);
      }
    }

    return b_;
  }

private:
  inline unsigned int index(unsigned int i, unsigned int j) const{
    return i*n_+j;
  }

  unsigned int m_;
  unsigned int n_;
  boost::shared_array<T> data_;
};

typedef Matrix<long double> MatrixLD;

#endif

