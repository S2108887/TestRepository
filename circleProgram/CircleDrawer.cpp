#include <cmath>

#include "CircleDrawer.h"
#include "ReadTool.h"

namespace ProjectName {

class CircleDrawer::I {
 public:
  I(CircleDrawer& p) : parent(p) {
    
  }

  void createIndexSets(AttributeContainer& aC,IndexContainer& iC) {
    //This only creates one index set at this time, but this might change later.
    createCircleSet(aC,iC);
  }

  void createShaderProgram(const std::string& dataLocation) {
    auto vertexCode = ReadTool::readWholeFile( getPath(dataLocation,"CircleVertex.glsl") );
    auto fragmentCode = ReadTool::readWholeFile( getPath(dataLocation,"CircleFragment.glsl") );

    auto& program = parent.circleShader;

    program.getName() = "CircleShader";

    program.compile(vertexCode,fragmentCode);

    program.bindAttributeLocation(0,"position");
    program.bindAttributeLocation(1,"radius");

    program.link();

    parent.colorUniform = program.getUniformLocation("color");
    parent.depthUniform = program.getUniformLocation("depth");
    parent.matrixUniform = program.getUniformLocation("theMatrix");
  }
  
 private:
  CircleDrawer& parent;

  static constexpr size_t CIRCLE_POINTS = 50;
  static constexpr double PI = 3.14159265359;

  void createCircleSet(AttributeContainer& aC,IndexContainer& iC) {
    IndexSet& circleSet = parent.circleSet;

    size_t nVertices = aC.getNumberOfVertices();
    circleSet.setOffset( iC.getNumberOfIndices() );
    
    addCircleAttributes(aC);
    addCircleIndices( iC, nVertices );

    circleSet.setMode(IndexSet::FAN);
    circleSet.setNumberOfIndices(CIRCLE_POINTS + 2);
  }

  void addCircleAttributes(AttributeContainer& c) {
    double R,angle,angleIncrement;
    R = 1.0;
    angle = 0.0;
    angleIncrement = 2*PI/CIRCLE_POINTS;
    GLfloat x = 0.0f,y = 0.0f;

    c.addAttribute<GLfloat>(0, {x,y} );
    c.addAttribute<GLubyte>(1, {0,0,0,255});

    for(unsigned int i = 0;i < CIRCLE_POINTS; ++i) {
      x = static_cast<GLfloat>( R*std::cos(angle) );
      y = static_cast<GLfloat>( R*std::sin(angle) );
      c.addAttribute<GLfloat>(0, {x,y} );

      c.addAttribute<GLubyte>(1, {255,0,0,255});

      angle += angleIncrement;
    }
  }

  void addCircleIndices(IndexContainer& c,size_t nVertices) {
    GLushort index = static_cast<GLushort> (nVertices);
    for( unsigned int i = 0; i < CIRCLE_POINTS + 1; ++i ) {
      c.add({index});
      ++index;
    }

    index = static_cast<GLushort>(nVertices + 1);
    c.add({index});
  }

  std::string getPath(const std::string& location,const std::string& fileName) {
    std::string result = location;
    result += "/";
    result += fileName;
    return result;
  }

};

#ifdef Cla
  #error "The macro Cla has already be defined, but I wanted to use it to reduce typing time :("
#else
  #define Cla CircleDrawer

Cla::Cla() {
  imp = std::unique_ptr<I>( new I(*this) );
}

Cla::~Cla() = default;

void Cla::createIndexSets(AttributeContainer& aC,IndexContainer& iC) {
  imp->createIndexSets(aC,iC);
}

void Cla::createShaderProgram(const std::string& dataLocation) {
  imp->createShaderProgram(dataLocation);
}

  #undef Cla
#endif

} //end of namespace
