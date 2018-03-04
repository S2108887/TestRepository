#include "ShaderProgram.h"
#include <vector>
#include <cstdio>
#include <stdexcept>

namespace ProjectName {

class ShaderProgram::I {
 
 public:
  I() {
    attributes.reserve(NUMBER_OF_ATTRIBUTES_RESERVED);
  }

  String& getName() {
    return name;
  }

  void compile(const String& vertexCode, const String& fragmentCode) {
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    std::printf( "Trying to compile shader program %s\n",name.c_str() );
    fillShader(vertexShader,vertexCode,true);
    fillShader(fragmentShader,fragmentCode,false);
    
    if(programNumber == 0) {
      programNumber = glCreateProgram();
    }
    glAttachShader(programNumber,vertexShader);
    glAttachShader(programNumber,fragmentShader);
  }
  

  void bindAttributeLocation(GLuint index,String text) {
    glBindAttribLocation( programNumber,index,text.c_str() );
    attributes.emplace_back(index, std::move(text) );
  }
  
  void link() {
    glLinkProgram(programNumber);

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(programNumber,GL_LINK_STATUS,&linkStatus);
    if(linkStatus == GL_TRUE) {
      std::printf("Linking shader program %s was successful.\n",name.c_str());
      deleteShaderObjects();
    }
    else {
      std::printf( "Linking shader program %s failed.\n",name.c_str() );
      printLinkLog();
      throw std::runtime_error("GLSL Linking error");
    }

    checkAttributeBindings();
  }


  GLint getUniformLocation(const String& uniformName) { 
    GLint result = glGetUniformLocation( programNumber,uniformName.c_str() );
    
    if(result == -1) {
      std::printf( "Could not obtain uniform location \"%s\".\n",uniformName.c_str() );
      throw std::runtime_error("glGetUniformLocation error");
    }

    return result;
  }
  
  void activate() {
    glUseProgram(programNumber);
  }

  void destroyProgram() {
    glDeleteProgram(programNumber);
    programNumber = 0;
  }


 private:
  static constexpr unsigned char NUMBER_OF_ATTRIBUTES_RESERVED = 10;
  
  GLuint programNumber{0}, vertexShader{0},fragmentShader{0};
  String name;
  

  class Attribute {
   public:
    GLuint index;
    String name;
    Attribute(GLuint ind,String na) : index(ind), name( std::move(na) ) {}
  };
  std::vector<Attribute> attributes;

  void fillShader(const GLuint& shader,const String& code, bool isVertex) {
    GLint length = code.length();
    const GLchar * source = (const GLchar *) code.c_str();
    glShaderSource(shader,1,&source,&length);

    glCompileShader(shader);

    checkCompilation(shader,isVertex);
  }

  void checkCompilation(const GLuint& shader, bool isVertex) {
    const char * text;
    if(isVertex) {
      text = "vertex";
    }
    else {
      text = "fragment";
    }
    
    GLint isCompiled = GL_FALSE;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&isCompiled);
    if(isCompiled == GL_TRUE) {
      std::printf("  compilation of %s shader was successful.\n",text);
    }
    else {
      std::printf("  Could not compile the %s shader.\n",text);
      std::string error = "Could not compile ";
      error += name;
      printCompileLog(shader);
      throw std::runtime_error(error);
    }
  }

  void printCompileLog(const GLuint& shader) {
    GLint length; 
    glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&length);
    auto container = createInfoLogContainer(length);
    char * infoLog = container.data();
    
    glGetShaderInfoLog(shader,(GLsizei) length,NULL,infoLog); 
    std::printf("Compile Info Log: %s\n",infoLog);
  }

  std::vector<char> createInfoLogContainer(size_t length) {
    std::vector<char> result;
    result.resize(length,'\0');
    return result;
  }

  void deleteShaderObjects() {
    glDetachShader(programNumber,vertexShader);
    glDetachShader(programNumber,fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vertexShader = 0;
    fragmentShader = 0;
  }


  void printLinkLog() {
    GLint length; 
    glGetProgramiv(programNumber,GL_INFO_LOG_LENGTH,&length);
    auto container = createInfoLogContainer(length);
    char * infoLog = container.data();

    glGetProgramInfoLog(programNumber,(GLsizei) length,NULL,infoLog); 
    std::printf("Link Info Log: %s\n",infoLog);
  }

  void checkAttributeBindings() {
    for(const auto& a : attributes) {
      checkAttributeBinding(a);
    }
  }
  void checkAttributeBinding(const Attribute& a) {
    GLint i =  glGetAttribLocation(programNumber,a.name.c_str());
    if(i == -1) {
      //This can happen if an attribute is declared in the .glsl file, but not used.
      std::printf( "WARNING: Could not bind vertex attribute %s.\n",a.name.c_str() );
    }
    else if(a.index != (GLuint) i ) {
      std::printf("Vertex attribute %s was bound to %d instead of %u",a.name.c_str(),i,a.index);
      throw std::runtime_error("ShaderProgram: vertex attribute binding error");
    }
  }
  
  
  
  
};

#ifdef Cla
  #error "The macro Cla has already be defined, but I wanted to use it to reduce typing time :("
#else
  #define Cla ShaderProgram

Cla::Cla() {
  imp = std::unique_ptr<I>( new I() );
}

Cla::~Cla() = default;

std::string& Cla::getName() {
  return imp->getName();
}

void Cla::compile(const String& vertexCode, const String& fragmentCode) {
  imp->compile(vertexCode,fragmentCode);
}

void Cla::bindAttributeLocation(GLuint index,String text) {
  imp->bindAttributeLocation(index,text);
}

void Cla::link() {
  imp->link();
}

GLint Cla::getUniformLocation(const String& uniformName) {
  return imp->getUniformLocation(uniformName);
}

void Cla::activate() {
  imp->activate();
}

void Cla::destroyProgram() {
  imp->destroyProgram();
}

  #undef Cla
#endif



}
