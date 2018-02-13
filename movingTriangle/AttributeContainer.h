#pragma once

#include <vector>
#include <glad/glad.h>

#include <stdexcept>
#include <cstdio>

namespace ProjectName {

class AttributeContainer {
 public:

  enum Type : GLenum {
    BYTE = GL_BYTE,
    UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
    SHORT = GL_SHORT,
    UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
    FIXED = GL_FIXED,
    FLOAT = GL_FLOAT
  };

  enum AttributeLength : unsigned char {
    ONE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4
  };

  void addAttributeType(Type type, bool normalized,AttributeLength length) {
    unsigned char typeSize, attributeSize;
    typeSize = getTypeSize(type);
    attributeSize = typeSize*length;
    checkDivisibleBy4(attributeSize);

    GLsizeiptr offset = (GLsizeiptr) vertexSize;
    vertexSize += attributeSize;
    
    
    attributeTypes.emplace_back(type,typeSize,normalized,length,offset);
  }

  void reserve(size_t numberOfVertices) {
    if(vertexSize == 0) {
      throw std::runtime_error(
        "Attribute types have to be added before calling AttributeContainer::reserve."
      );
    }

    attributeBuffer.resize(numberOfVertices*vertexSize);
    maxVertices = numberOfVertices;
  }

  template<class T>
  void addAttribute(unsigned char index, std::initializer_list<T> values) {
    auto& a = attributeTypes[index];
    
    additionCheck( a,sizeof(T),values.size() );

    char* insertLocation = &attributeBuffer[a.counter*vertexSize + a.offset];
    T * p = reinterpret_cast<T*>(insertLocation);
    
    for(const T& t: values) {
      *p = t;
      ++p;
    }
    ++a.counter;
  }

  void printData() {
    std::printf("attribute bytes:\n  ");
    for(auto c : attributeBuffer) {
      std::printf("(%hhu)",c);
    }
    std::printf("\n");
  }


  void initialize() { //Requires an OpenGL context;
    if( attributeBuffer.empty() ) {
      throw std::runtime_error(
        "AttributeContainer::initialize() was called, but no attributes have been added."
      );
    }
    
    size_t numberOfVertices = attributeTypes[0].counter;
    for(const auto & a : attributeTypes) {
      if(a.counter != numberOfVertices) {
        throw std::runtime_error("The vertex attribute arrays have different lengths.");
      }
    }
    
    glGenBuffers(1,&attributeBufferName);
    glBindBuffer(GL_ARRAY_BUFFER,attributeBufferName);

    sendAttributesToGPU();
    
    for(unsigned char i = 0; i < attributeTypes.size(); ++i) {
      setVertexAttributePointer(i,attributeTypes[i]);
    }
    glBindBuffer(GL_ARRAY_BUFFER,0);
  }


   
 
 private:
  unsigned short vertexSize{0};

  size_t maxVertices{0};

  class AttributeInfo {
   public: 
    Type type;
    unsigned char typeSize, length;
    bool normalized{false};
    GLsizeiptr offset{0};

    size_t counter{0};

    AttributeInfo() {
      
    }
    AttributeInfo(Type t,unsigned char tSize,bool norma,unsigned char len,GLsizeiptr offs):
      type(t),typeSize( tSize ),length(len),normalized(norma),offset(offs)
    {
      
    }
  };

  std::vector<AttributeInfo> attributeTypes;
  
  std::vector<char> attributeBuffer;
  GLuint attributeBufferName;
  
  static unsigned char getTypeSize(Type t) {
    //These values can be found in the OpenGL ES specification. 
    //For instance, the OpenGL ES 3.1 specification lists the size of the OpenGL types in table
    //2.2.
    switch(t) {
      case BYTE           : return 1;
      case UNSIGNED_BYTE  : return 1;
      case SHORT          : return 2;
      case UNSIGNED_SHORT : return 2;
      case FIXED          : return 4;
      case FLOAT          : return 4;
    }
  }

  void checkDivisibleBy4(unsigned char attributeSize) {
    if(attributeSize%4 != 0) {
      throw std::runtime_error("typeSize*length should be a multiple of 4");
      //This may not be required for all video drivers, but I have read the following at
      //https://www.khronos.org/opengl/wiki/Common_Mistakes :
      //``if you are interested, most GPUs like chunks of 4 bytes.''
      //However, this was in a section about texture alignment, so it may not hold for vertex
      //attributes, but to be safe, I still want to restrict the attribute sizes to multiples of 4.
    }
  }

  void additionCheck(const AttributeInfo& a,size_t typeSize, size_t length) {
    if( length != a.length ) {
      std::printf("Expected an attribute array of length %u, but received %zu\n",a.length,length);
      throw std::runtime_error("AttributeContainer length mismatch");
    }

    if(a.counter >= maxVertices) {
      throw std::runtime_error("AttributeContainer has received too many attributes.\n");
    }
    if( typeSize != a.typeSize ) {
      std::printf("The type size should be %u bytes, but it was %zu\n",a.typeSize,typeSize);
      throw std::runtime_error("AttributeContainer type size mismatch");
    }
  }

  void sendAttributesToGPU() {
    
    glBufferData(
      GL_ARRAY_BUFFER,
      attributeBuffer.size(),
      (void *) attributeBuffer.data(),
      GL_STATIC_DRAW
    );
    
  }

  void setVertexAttributePointer(unsigned char index,const AttributeInfo& a) {
    glVertexAttribPointer(
      index,a.length,a.type,a.normalized,vertexSize,(const GLvoid *) a.offset
    );
  }
};

}
