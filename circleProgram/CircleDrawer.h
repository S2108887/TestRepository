#pragma once

#include "IndexSet.h"
#include "AttributeContainer.h"
#include "IndexContainer.h"
#include "ShaderProgram.h"

#include "Vec2.h"

namespace ProjectName {

class CircleDrawer {
 public:
  CircleDrawer();
  ~CircleDrawer();

  void setScreenSize(int width,int height) {
    aspectRatio = (double) height/width;
  }
  
  void createIndexSets(AttributeContainer& aC, IndexContainer& iC);

  //The following methods require an OpenGL context.

  void createShaderProgram(const std::string& dataLocation);

  void activateShaderProgram() {
    circleShader.activate();
  }

  void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    glUniform4f(colorUniform,r,g,b,a);
  }

  void setRadius(double r) {
    matrix[0] = (GLfloat) ( aspectRatio*r );
    matrix[4] = (GLfloat) r;
  }

  void setPosition(const Vec2& v) {
    setPosition( v.getX(),v.getY() );
  }
  
  void setPosition(double x, double y) {
    matrix[6] = (GLfloat) ( aspectRatio*x );
    matrix[7] = (GLfloat) y;
  }

  void setDepth(double d) {
    glUniform1f(depthUniform,(GLfloat) d);
  }

  void draw() {
    glUniformMatrix3fv(matrixUniform,1,false,matrix);
    circleSet.draw();
  }


 private:
  GLfloat matrix[9] {
    1.0f,0.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,0.0f,1.0f
  };
  GLint colorUniform,matrixUniform,depthUniform;
  
  ShaderProgram circleShader;
  IndexSet circleSet;
  double aspectRatio{1.0};

  class I;
  std::unique_ptr<I> imp;
};

}
