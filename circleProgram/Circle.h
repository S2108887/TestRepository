#pragma once

#include <atomic>

#include "Vec2.h"
#include "CircleDrawer.h"

namespace ProjectName {

class Circle {
 public:
  Circle() {
  }

  void setColor(GLfloat r, GLfloat g, GLfloat b,GLfloat a) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = a;
  }

  Vec2& getPosition() {
    return position;
  }
  
  Vec2& getVelocity() {
    return velocity;
  }

  void setRadius(double r) {
    radius = r;
  }

  void setMass(double m) {
    mass = m;
  }

  void setDepth(double d) {
    depth = d;
  }

  void move() {
    position += velocity;
  }

  void collide(Circle& other) {
    Vec2 dr = position;
    dr -= other.position;
    double drLength = dr.norm();
    double s = radius + other.radius - drLength;
    
    if(s > 0) {
      s += EPS;
      normalize(dr,drLength);
      position.add(0.5*s,dr);
      other.position.add(-0.5*s,dr);

      adjustVelocitiesAfterCollision(other,dr);
    }
  }

  void handleBounds(double xMax, double yMax) {
    auto& r = position;
    auto& v = velocity;

    //I assume that radius <= xMax and radius <= yMax.

    double x = r.getX(), y = r.getY();

    if( std::abs(x) + radius > xMax) {
      double s = getSign(x), vx = v.getX();
      x = s*( xMax - radius - EPS );
      r.setX(x);
      if(vx*s > 0) {
        v.setX(-vx);
      }
    }
    if( std::abs(y) + radius > yMax) {
      double s = getSign(y), vy = v.getY();
      y = s*( yMax - radius - EPS );
      r.setY(y);
      if(vy*s > 0) {
        v.setY(-vy);
      }
    }
  }

  void draw(CircleDrawer& d) const {
    d.setColor(color[0],color[1],color[2],color[3]);
    d.setRadius(radius);
    d.setPosition( position );

    d.setDepth(depth);

    d.draw();
  }

 private:
  static constexpr double EPS = 0.00001; //A small number.
  
  double radius, mass;

  GLfloat color[4] {1.0f,0.0f,0.0f,1.0f};

  Vec2 position,velocity;
  double depth{0.5};

  double getSign(double d) {
    if(d >= 0.0) {
      return 1.0;
    }
    else {
      return -1.0;
    }
  }

  void normalize(Vec2& v, double vLength) {
    if(vLength > EPS) {
      v.scal(1.0/vLength);
    }
    else {
      v.set(1.0,0.0);
    }
  }

  void adjustVelocitiesAfterCollision(Circle& other,Vec2& n) {
    //n is the normal vector pointing from the other circle towards this circle.

    double dv = velocity.dot(n) - other.velocity.dot(n);
    if(dv >= 0) {
      //The circles are already moving away from eachother.
      return;
    }
    else {
      double dv2 = 2*mass/(mass + other.mass)*dv;
      other.velocity.add(dv2,n);
      velocity.add( -other.mass/mass*dv2,n );
    }
  }

};

}
