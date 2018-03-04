#pragma once

#include <glad/glad.h>
#include <memory>
#include <string>

namespace ProjectName {

class ShaderProgram {
  using String = std::string;

 public:
  ShaderProgram();
  ~ShaderProgram();

  String& getName();

  //The following methods require an OpenGL context.
  void compile(const String& vertexCode, const String& fragmentCode); 
  void bindAttributeLocation(GLuint index,String text);
  void link();
  GLint getUniformLocation(const String& uniformName);
  void activate();
  void destroyProgram();

 private:
  
  class I;
  std::unique_ptr<I> imp;
};

}
