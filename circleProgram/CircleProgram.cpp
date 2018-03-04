#include "GLWindow.h"

#include "sleep.h"

#include <cstdio>
#include <string>
#include <atomic>

#include <SDL.h>

#include "ShaderProgram.h"
#include "AttributeContainer.h"
#include "IndexContainer.h"
#include "IndexSet.h"
#include "ReadTool.h"
#include "CircleDrawer.h"
#include "Circle.h"

#include <list>

#include "NumberGenerator.h"

namespace ProjectName {

class CircleProgram: public Renderer {
 public:


  void setDataLocation(const char * location) {
    dataLocation = location;
  }

  void start() {
    std::printf("Hello, World!\n");
    
    window.initialize();
    int width,height;
    width = window.getScreenWidth();
    height = window.getScreenHeight();

    yMax = 1.0;
    xMax = (double) width/height;

    std::printf("(Width,Height,Frequency) = (%d,%d,%d)\n",
      width,
      height,
      window.getScreenFrequency()
    );

    circleDrawer.setScreenSize(width,height);

    defineAttributeTypes();

    createIndexSetsAndAttributes();

    addRandomCircle();
    addRandomCircle();
    addRandomCircle();

    
    window.setRenderer(this);
    window.start();

    space = false;
    
    processEvents();
    
    window.stop();

    sleep(50); //Waiting here prevents the weird segmentation fault that occurrs at the end
    //of the program.
    
  }

  void initializeRendering() override {
    printOpenGLInformation();
    
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    
    
    glEnable(GL_DEPTH_TEST);
    glClearDepthf(0.0f);
    glDepthFunc(GL_GEQUAL);
    glDepthRangef(0.0f,1.0f);


    createShaderPrograms();

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    attributeContainer.initialize();
    indexContainer.initialize();

    //shaderProgram.activate();

    printOpenGLError( glGetError() );
  }
  
  void render() override {
    update();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    shaderProgram.activate();
    
   // x += 0.0015;
    x += 4.0/60.0;
    if(x > 2.0) {
      x = -2.0;
    }
    x = 0.0;
    
    matrix[6] = (GLfloat) x;
    
    glUniformMatrix3fv(matrixUniform,1,false,matrix);

    triangleSet.draw();

    circleDrawer.activateShaderProgram();

    drawCircles();

    
    
    /*
    auto error = glGetError();
    
    if( error != GL_NO_ERROR) {
      //printOpenGLError(error);
      stopBoolean = true;
    }
    */
  }

 private:
  static constexpr unsigned char EVENT_SLEEP_TIME = 5, ADD_CIRCLE_DELAY=20;
  static constexpr double 
    MIN_RADIUS             = 0.05,
    MAX_RADIUS             = 0.3,
    MAX_VELOCITY_COMPONENT = 0.02
  ;

  std::string dataLocation{"."};
  GLWindow window;
  std::atomic<bool> stopBoolean{false};

  ShaderProgram shaderProgram;
  AttributeContainer attributeContainer;
  IndexContainer indexContainer;
  IndexSet triangleSet;

  CircleDrawer circleDrawer;

  std::atomic<bool> space;
  unsigned int counter{0};

  double xMax,yMax;

  double x{0.0};
  GLfloat matrix[9] {  
    1,0,0,
    0,1,0,
    0,0,1
  };
  GLint matrixUniform;

  std::list< std::unique_ptr<Circle> > circleList;

  NumberGenerator rng;

  void printOpenGLInformation() {
    std::printf( "OpenGL Version  : %s\n",glGetString(GL_VERSION) );
    std::printf( "OpenGL Vendor   : %s\n",glGetString(GL_VENDOR) );
    std::printf( "OpenGL Renderer : %s\n",glGetString(GL_RENDERER) );
  }

  void defineAttributeTypes() {
    using C = AttributeContainer;
    C& c = attributeContainer;

    c.addAttributeType( C::FLOAT,false,C::TWO);
    c.addAttributeType( C::UNSIGNED_BYTE,true,C::FOUR);
  }

  void createIndexSetsAndAttributes() {
    attributeContainer.reserve(150);
    indexContainer.reserve(200);

    createTriangleSet();
    circleDrawer.createIndexSets(attributeContainer,indexContainer);
  }

  void createTriangleSet() {
    auto& c = attributeContainer;
    c.addAttribute<GLfloat>(0, {-1.0f,-0.5} );
    c.addAttribute<GLfloat>(0, { 1.0f,-0.5} );
    c.addAttribute<GLfloat>(0, { 0,1.0f} );

    GLubyte b = 255;
    c.addAttribute<GLubyte>(1, {b,0,0,b} );
    c.addAttribute<GLubyte>(1, {0,b,0,b} );
    c.addAttribute<GLubyte>(1, {0,0,b,b} );

    indexContainer.add({0,1,2});

    triangleSet.setMode(IndexSet::TRIANGLE);
    triangleSet.setNumberOfIndices(3);
    triangleSet.setOffset(0);
  }




  void update() {
    ++counter;
    if(counter > ADD_CIRCLE_DELAY  && space) {
      counter = 0;
      addRandomCircle();
    }

    moveCircles();

    handleCollisions();
  }

  void moveCircles() {
    for(auto& c : circleList) {
      c->move();
    }
  }

  void handleCollisions() {
    auto i = circleList.begin();
    decltype(i) j;
    for(; i != circleList.end(); ++i) {
      j = i;
      ++j;
      for(; j != circleList.end(); ++j) {
        (**i).collide(**j);
      }
      (**i).handleBounds(xMax,yMax);
    }
  }

  void drawCircles() {
    for(const auto& c : circleList) {
      c->draw( circleDrawer );
    }
  }

  void addRandomCircle() {
    Circle* p = new Circle();
    Circle& c = *p;
    c.setColor( (GLfloat) rng(),(GLfloat) rng(),(GLfloat) rng(),1.0f );

    double R = random(MIN_RADIUS,MAX_RADIUS);
    c.setRadius(R);
    c.setMass(R*R);

    double maximumX = xMax - R,maximumY = yMax - R;
    c.getPosition().set( random(-maximumX,maximumX), random(-maximumY,maximumY) );

    double v = MAX_VELOCITY_COMPONENT;
    c.getVelocity().set( random(-v,v), random(-v,v) );

    circleList.emplace_back(p);
  }

  double random(double min, double max) {
    return min + rng()*(max - min);
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
    else if(event.type == SDL_KEYUP) {
      handleKeyRelease( event.key );
    }
  }

  void handleKeyPress(const SDL_KeyboardEvent& event) {
    if (event.repeat != 0) {
      return;
    }
    
    if( getKey(event) == SDLK_ESCAPE ) {
      stopBoolean = true;
    }
    else if(getKey(event) == SDLK_SPACE) {
      space = true;
    }
  }

  SDL_Keycode getKey(const SDL_KeyboardEvent& event) {
    return event.keysym.sym;
  }

  void handleKeyRelease(const SDL_KeyboardEvent& event) {
    if(getKey(event) == SDLK_SPACE) {
      space = false;
    }
  }
  
  
  std::string readFile(const std::string& fileName) {
    auto path = getPath(fileName);
    return ReadTool::readWholeFile(path);
  }

  std::string getPath(const std::string& fileName) {
    std::string result = dataLocation;
    result += "/";
    result += fileName;
    return result;
  }

  void createShaderPrograms() {
    shaderProgram.getName() = "TestProgram";
    
    auto vertexCode = readFile("TestVertex.glsl");
    auto fragmentCode = readFile("TestFragment.glsl");

    shaderProgram.compile(vertexCode,fragmentCode);

    shaderProgram.bindAttributeLocation(0,"position");
    shaderProgram.bindAttributeLocation(1,"color");
    shaderProgram.link();

    matrixUniform = shaderProgram.getUniformLocation("theMatrix");
    
    circleDrawer.createShaderProgram(dataLocation);
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
    
    
  SDL_Init(0);

  std::printf("Hello,World!\n");
  program.start();

  SDL_Quit(); //This causes a weird segmentation fault unless I wait at the end of program.start().
              //I have no idea why this happens.

  return 0;
}
