#pragma once

#include <cmath>

namespace ProjectName {

class Vec2 {
 public:
  
  Vec2() = default;

  Vec2(double a, double b) {
    x = a;
    y = b;
  }

  void setX(double d) {
    x = d;
  }

  void setY(double d) {
    y = d;
  }
  
  double getX() const {
    return x;
  }

  double getY() const {
    return y;
  }

  void set(double a, double b) {
    x = a;
    y = b;
  }

  double dot() const {
    return x*x + y*y;
  }

  double dot(const Vec2& other) const {
    return x*other.x + y*other.y;
  }

  void operator+=(const Vec2& other) {
    x = x + other.x;
    y = y + other.y;
  }

  void add(double a,const Vec2& v) { // += a*v
    x += a*v.x;
    y += a*v.y;
  }

  void operator-=(const Vec2& other) {
    x = x - other.x;
    y = y - other.y;
  }

  double norm() const {
    return std::sqrt( dot() );
  }

  void scal(double d) {
   x = d*x;
   y = d*y;
  }

 private:
  double x,y;
};

}
