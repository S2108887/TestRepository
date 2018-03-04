#pragma once

#include <glad/glad.h>

class IndexSet {
 public:
  enum DrawMode : GLenum  {
    STRIP = GL_TRIANGLE_STRIP,
    FAN = GL_TRIANGLE_FAN,
    TRIANGLE = GL_TRIANGLES
  };

  IndexSet() {
    
  }

  IndexSet(DrawMode mod,GLsizei numberOfIndices,GLsizeiptr offs) :
    mode(mod),count(numberOfIndices),offset(offs)
  {
    
  }

  void setMode(DrawMode mod) {
    mode = mod;
  }

  void setNumberOfIndices(GLsizei c) {
    count = c;
  }

  void setOffset(GLsizeiptr offs) {
    offset = offs*sizeof(GLushort);
  }

  void draw() { //Requires an OpenGL context.
    glDrawElements(mode,count,GL_UNSIGNED_SHORT,(const GLvoid *) offset  );
  }

 private:
  DrawMode mode{TRIANGLE};
  GLsizei count{0};
  GLsizeiptr offset{0};
};
