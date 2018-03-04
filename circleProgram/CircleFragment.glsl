#version 100

precision mediump float;

varying float fragmentRadius;

uniform vec4 color;

const float R_START = 0.6;
const float factor = 1.0/(1.0 - R_START);

void main() {
  if( fragmentRadius < R_START ) {
    discard;
  }
  else {
    gl_FragColor=vec4( factor*(fragmentRadius - R_START)*color.xyz,color.w );
  }
}
