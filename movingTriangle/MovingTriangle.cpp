#include "GLWindow.h"

#include "sleep.h"

#include <cstdio>
#include <string>
#include <fstream>
#include <atomic>

#include <SDL.h>
#include <glad/glad.h>

#include <ShaderProgram.h>
#include <AttributeContainer.h>
#include <IndexContainer.h>

namespace ProjectName {

class CircleProgram: public Renderer {
 public:


  void setDataLocation(const char * location) {
    dataLocation = location;
  }

  void start() {
    std::printf("Hello, World!\n");
    SDL_Init(0);
    
    window.initialize();

    std::printf("(Width,Height,Frequency) = (%d,%d,%d)\n",
      window.getScreenWidth(),
      window.getScreenHeight(),
      window.getScreenFrequency()
    );

    fillAttributeContainer();
    fillIndexContainer();


    //indexContainer.printIndices();
    //attributeContainer.printData();
    
    window.setRenderer(this);
    window.start();
    
    processEvents();
    
    window.stop();
    
    SDL_Quit();
  }

  void initializeRendering() override {
    printOpenGLInformation();
    
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClearDepthf(0.0f);
    glDisable(GL_DEPTH_TEST);


    createShaderProgram();

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    attributeContainer.initialize();
    indexContainer.initialize();

    

    shaderProgram.activate();

    printOpenGLError( glGetError() );
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    printOpenGLError( glGetError() );
    
    x = 0.0;
    
    matrix[2] = (GLfloat) x;
    
    glUniformMatrix3fv(matrixUniform,1,false,matrix);
    printOpenGLError( glGetError() );
    
    glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_SHORT,0);
    printOpenGLError( glGetError() );
    
    
  }
  
  void render() override {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    x += 0.0015;
    if(x > 1.5) {
      x = -1.5;
    }
    
    matrix[6] = (GLfloat) x;
    
    glUniformMatrix3fv(matrixUniform,1,false,matrix);
    
    glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_SHORT,0);
    
    /*
    auto error = glGetError();
    
    if( error != GL_NO_ERROR) {
      //printOpenGLError(error);
      stopBoolean = true;
    }
    */
  }

 private:
  static constexpr unsigned char EVENT_SLEEP_TIME = 5;

  std::string dataLocation{"."};
  GLWindow window;
  std::atomic<bool> stopBoolean{false};

  ShaderProgram shaderProgram;
  AttributeContainer attributeContainer;
  IndexContainer indexContainer;

  double x{0.0};
  GLfloat matrix[9] {  
    1,0,0,
    0,1,0,
    0,0,1
  };
  GLint matrixUniform;

  void printOpenGLInformation() {
    std::printf( "OpenGL Version  : %s\n",glGetString(GL_VERSION) );
    std::printf( "OpenGL Vendor   : %s\n",glGetString(GL_VENDOR) );
    std::printf( "OpenGL Renderer : %s\n",glGetString(GL_RENDERER) );
  }


  void fillAttributeContainer() {
    using C = AttributeContainer;
    C& c = attributeContainer;

    c.addAttributeType( C::FLOAT,false,C::TWO);
    c.addAttributeType( C::UNSIGNED_BYTE,true,C::FOUR);

    c.reserve(3);

    GLfloat f = 0.5;
    c.addAttribute<GLfloat>(0, {-f,0} );
    c.addAttribute<GLfloat>(0, { f,0} );
    c.addAttribute<GLfloat>(0, { 0,f} );


    GLubyte b = 255;
    c.addAttribute<GLubyte>(1, {b,0,0,b} );
    c.addAttribute<GLubyte>(1, {0,b,0,b} );
    c.addAttribute<GLubyte>(1, {0,0,b,b} );
  }

  void fillIndexContainer() {
    indexContainer.reserve(3);
    indexContainer.add({0,1,2});
  }

  void processEvents() {
    while(!stopBoolean) {
      SDL_Event event;
      while( SDL_PollEvent(&event) ) {
        processEvent(event);
      }
      sleep(EVENT_SLEEP_TIME);
    }
  }

  void processEvent(const SDL_Event& event) {
    if(event.type == SDL_KEYDOWN) {
      handleKeyPress( event.key );
    }
  }

  void handleKeyPress(const SDL_KeyboardEvent& event) {
    if( getKey(event) == SDLK_ESCAPE ) {
      stopBoolean = true;
    }
  }

  SDL_Keycode getKey(const SDL_KeyboardEvent& event) {
    return event.keysym.sym;
  }


  std::string readFile(const std::string& fileName) {
    auto path = getPath(fileName);

    std::string result;
    result.reserve(1000);

    std::ifstream stream;
    stream.open(path);
    char c;
    while( stream.good() ) {
      stream.get(c);
      result += c;
    }
    return result;
  }

  std::string getPath(const std::string& fileName) {
    std::string result = dataLocation;
    result += "/";
    result += fileName;
    return result;
  }

  void createShaderProgram() {
    shaderProgram.getName() = "TestProgram";
    
    auto vertexCode = readFile("TestVertex.glsl");
    auto fragmentCode = readFile("TestFragment.glsl");

    shaderProgram.compile(vertexCode,fragmentCode);

    shaderProgram.bindAttributeLocation(0,"position");
    shaderProgram.bindAttributeLocation(1,"color");
    shaderProgram.link();

    matrixUniform = shaderProgram.getUniformLocation("theMatrix");
  }

  void printOpenGLError(GLenum error) {
    //GLenum error = glGetError();

    if(error == GL_NO_ERROR) {
      std::printf("No OpenGL error has occurred.\n");
      return;
    }
    
    std::printf("OpenGL error: ");
    
    switch(error) {
      case GL_INVALID_ENUM: std::printf("Invalid enum\n"); break;
      case GL_INVALID_VALUE: std::printf("Invalid value\n"); break;
      case GL_INVALID_OPERATION: std::printf("Invalid operation\n"); break;
      case GL_INVALID_FRAMEBUFFER_OPERATION: std::printf("Invalid frame operation\n"); break;
      case GL_OUT_OF_MEMORY: std::printf("Out of memory\n"); break;
    }
  }
  
};

} //End of namespace

int main(int n,char** arguments) {
  ProjectName::CircleProgram program;
  
  //The first command line argument should be the location containing the .glsl files.

  if(n > 1) {
    program.setDataLocation(arguments[1]);
  }


  program.start();


  return 0;
}
