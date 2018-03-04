project('CircleProgram','cpp',
  default_options : ['cpp_std=c++14', 'warning_level=3', 'buildtype=release']
)

src=[
  'CircleProgram.cpp',
  'GLWindow.cpp',
  'glad.cpp',
  'ShaderProgram.cpp',
  'ReadTool.cpp',
  'CircleDrawer.cpp',
  'NumberGenerator.cpp'
]

SDL = dependency('sdl2' ,version : '>=2.0.8')

threads = dependency('threads')

extraIncludeDirectories = include_directories('gladInclude')

program = executable('a.out',src,dependencies : [SDL,threads],
  include_directories: extraIncludeDirectories,
  cpp_pch : 'pch/PrecompiledHeader.hpp'
)
