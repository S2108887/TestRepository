#pragma once

#include <glad/glad.h>
#include<vector>

namespace ProjectName {
 
class IndexContainer {
  using Type = GLushort;
  //I have never had to use more than 2^16 indices, but if we need this, then the indices could be 
  //stored as GLuint.  This requires the OpenGL ES extension GL_OES_element_index_uint. 
  //This extension should be common as it exists since 2005.
  //It should not be very hard to check whether this extension is available.

  //Another approach is to use glDrawElementsBaseVertex which requires OpenGL ES 3.2. 
  //However, this version is not supported by most Raspberry-Pi-like computers. In addition, 
  //computers that do support version 3.2 will also support Vulkan, the successor of OpenGL, 
  //eventually.
 
 public:
  void reserve(size_t numberOfIndices) {
    indexBuffer.reserve(numberOfIndices);
  }

  void add(std::initializer_list<Type> indices) {
    for(const Type& index : indices) {
      indexBuffer.push_back(index);
    }
  }

  void printIndices() {
    std::printf("indices:\n  ");

    for(auto s : indexBuffer) {
      std::printf("(%u)",s);
    }

    std::printf("\n");
  }

  void initialize() { //Requires an OpenGL context.
    sendIndicesToGPU();
  }

 private:
  std::vector<Type> indexBuffer;

  GLuint indexBufferName{0};

  void sendIndicesToGPU() {
    glGenBuffers(1,&indexBufferName);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBufferName);
    glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      indexBuffer.size()*sizeof(Type),
      (const GLvoid *) indexBuffer.data(),
      GL_STATIC_DRAW
    );
    
  }
  
};

}
