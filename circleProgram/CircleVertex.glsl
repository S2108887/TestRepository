#version 100

attribute vec2 position;
attribute float radius;

varying float fragmentRadius;

uniform mat3 theMatrix;
uniform float depth;

void main() {
  vec3 temp = theMatrix*vec3(position,1.0);
  gl_Position = vec4(temp.xy,depth,1.0);
  fragmentRadius = radius;
}
