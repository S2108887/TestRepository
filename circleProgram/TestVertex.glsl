#version 100

attribute vec2 position;
attribute vec4 color;

varying  vec4 fragmentColor;

uniform mat3 theMatrix;

void main() {
  vec3 temp = theMatrix*vec3(position,1.0);
  gl_Position = vec4(temp.xy,0.0,1.0);
  //gl_Position = vec4(position,0.0,1.0);
  fragmentColor = color;
}
