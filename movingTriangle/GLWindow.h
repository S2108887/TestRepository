#pragma once

#include<memory>

#include "Renderer.h"


namespace ProjectName {

class GLWindow {
 public:
  GLWindow();
  ~GLWindow();

  void initialize(); 
  //Initializes the video subsystem of SDL2 and obtains screen information.

  int getScreenWidth() const;
  int getScreenHeight() const;
  int getScreenFrequency() const;

  void setRenderer(Renderer * r);

  void start();

  void stop();

 private:
  class I;
  std::unique_ptr<I> imp;
  
};
 


}
