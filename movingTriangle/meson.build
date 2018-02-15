project('SDLTest','cpp',
  default_options : ['cpp_std=c++14', 'warning_level=2', 'buildtype=release']
)
src=['MovingTriangle.cpp','GLWindow.cpp','glad.cpp','ShaderProgram.cpp']

SDL = dependency('sdl2' ,version : '>=2.0.7')

threads = dependency('threads')

extraIncludeDirectories = include_directories('gladInclude')

program = executable('a.out',src,dependencies : [SDL,threads],
  include_directories: extraIncludeDirectories,
  cpp_pch : 'pch/PrecompiledHeader.hpp'
)

#  test('Bladiebla',program, timeout: 3600)
