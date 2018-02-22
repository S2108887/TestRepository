#include "GLWindow.h"

#include <cstdio>
#include <atomic>

#include <thread>

#include <stdexcept>

#include<glad/glad.h>
#include <SDL.h>

namespace ProjectName {
  
class GLWindow::I {
 public:
  I() {
    
  }

  void initialize() {
    if( !SDL_WasInit(SDL_INIT_VIDEO) ) {
      initializeVideoSubsystem();
    }

    obtainScreenInformation();
  }

  int getScreenWidth() const {
    return screenWidth;
  }

  int getScreenHeight() const {
    return screenHeight;
  }

  int getScreenFrequency() const {
    return screenFrequency;
  }

  void start() {
    if(renderer == nullptr) {
      throwMissingRendererError();
    }
    
    stopBoolean = false;

    createWindow();

    renderThread = std::thread( [this]() { renderThreadFunction(); } );
  }

  void setRenderer(Renderer * r) {
    renderer = r;
  }

  void stop() {
    stopBoolean = true;
    renderThread.join();
  }

 private:
  Renderer * renderer{nullptr};
  SDL_Window * window{nullptr};
  SDL_GLContext context{nullptr};

  std::atomic<bool> stopBoolean{true};
  std::thread renderThread;

  int screenWidth{0}, screenHeight{0}, screenFrequency{0};

  void initializeVideoSubsystem() {
    std::printf("Initializing the video subsystem.\n");
    if( SDL_InitSubSystem(SDL_INIT_VIDEO) != 0 ) {
      std::printf("The video subsystem could not be initialized.\n");
      throw std::runtime_error("SDL initialization error");
    }
  }

  void obtainScreenInformation() {
    int n = SDL_GetNumVideoDisplays();
    if(n == 1) {
      std::printf("There is one video display.\n");
    }
    else if(n > 0) {
      std::printf(
        "There are %d video displays, but this program will naively use the first\n"
        "display it can find.\n",n
      );
    }
    else {
      std::printf("Could not obtain a video display.\n");
      SDL_Quit();
      throw std::runtime_error("No display");
    }

    obtainDisplayInformation(0);
  }

  void obtainDisplayInformation(int displayNumber) { 

    SDL_DisplayMode desktopMode;
    SDL_GetDesktopDisplayMode(displayNumber,&desktopMode);
    
    screenWidth = desktopMode.w;
    screenHeight = desktopMode.h;
    screenFrequency = desktopMode.refresh_rate;
  }
  
  void throwMissingRendererError() {
    std::printf(
      "Method start of GLWindow is called without a specified renderer.\n"
      "The method setRenderer should be used.\n"
    );
    SDL_Quit();
    throw std::runtime_error("GLWindow::start() without renderer");
  }

  void throwWindowCreationError() {
    std::printf(
      "Could not create a window.\n"
      "SDL error message: %s\n",SDL_GetError()
    );
    SDL_Quit();
    throw std::runtime_error("Window creation failed");
  }
  
  void createWindow() {
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    
 
    window = SDL_CreateWindow(
      "NameOfWindow",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      screenWidth,
      screenHeight,
      SDL_WINDOW_OPENGL|
      SDL_WINDOW_FULLSCREEN|
      SDL_WINDOW_BORDERLESS
    );

    if(!window) {
      throwWindowCreationError();
    }


  }
  
  void renderThreadFunction() {
    createContext();

    makeContextCurrent();

    setSwapInterval();//The swap interval can only be set after obtaining a valid current context.

    checkHardwareAcceleration();
    
    loadOpenGLFunctions();

    renderer->initializeRendering();
    
    while(!stopBoolean) {
      renderer->render();
      SDL_GL_SwapWindow(window);
    }
  }

  void createContext() {
    context = SDL_GL_CreateContext(window);
    if(!context) {
      throwContextCreationError();
    }
  }

  void throwContextCreationError() {
    std::printf(
      "Could not create the OpenGL context.\n"
      "SDL error message: %s\n",SDL_GetError()
    );
    SDL_Quit();
    throw std::runtime_error("Context creation error");
  }

  void makeContextCurrent() {
    if( SDL_GL_MakeCurrent(window,context) != 0 ) {
      throwMakeCurrentError();
    }
  }

  void throwMakeCurrentError() {
    std::printf(
      "Could not make the OpenGL context current.\n"
      "SDL error message: %s\n",SDL_GetError()
    );
    SDL_Quit();
    throw std::runtime_error("GL_MakeCurrent error");
  }

  void setSwapInterval() {
    if(!trySettingSwapInterval(-1) ) {
      trySettingSwapInterval(1);
    }

    std::printf( "The swap interval has been set to %d\n",SDL_GL_GetSwapInterval() );
  }

  bool trySettingSwapInterval(int i) {
    if( SDL_GL_SetSwapInterval(i) != 0 ) {
      std::printf(
        "Swap interval %d is not supported.\n"
        "  SDL message: %s\n",
        i,
        SDL_GetError()
      );
      SDL_ClearError();
      return false;
    }
    return true;
  }

  void checkHardwareAcceleration() {
    int i = 0;
    i = SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL,&i);

    if(i == 1) {
      std::printf("There is hardware acceleration.\n");
    }
    else {
      std::printf("There is no hardware acceleration.\n");
    }
  }

  void loadOpenGLFunctions() {
    gladLoadGLES2Loader( (GLADloadproc) &SDL_GL_GetProcAddress );
  }

}; //end of class I

GLWindow::GLWindow() {
  imp = std::unique_ptr<I>( new I() );
}

GLWindow::~GLWindow() = default;

void GLWindow::initialize() {
  imp->initialize();
}

int GLWindow::getScreenWidth() const {
  return imp->getScreenWidth();
}
int GLWindow::getScreenHeight() const {
  return imp->getScreenHeight();
}
int GLWindow::getScreenFrequency() const {
  return imp->getScreenFrequency();
}

void GLWindow::start() {
  imp->start();
}

void GLWindow::stop() {
  imp->stop();
}
 
void GLWindow::setRenderer( Renderer * e ) {
  imp->setRenderer(e);
}

}
