#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
uniform mat4 MVP;

out vec3 modelpos;
out vec3 vertex_color;
void main(){
  gl_Position = MVP * vec4(vertexPosition_modelspace,1);
  modelpos.xyz = vertexPosition_modelspace;
  //Vertex color set randomly, just so we can differentiate triangles from one another
  vertex_color.x = 0.5 + 0.5 * fract(sin(dot(modelpos.xy, vec2(12.9898, 78.233)))* 43758.5453);
  vertex_color.y = 0.5 + 0.5 * fract(sin(dot(modelpos.yz, vec2(12.9898, 78.233)))* 43758.5453);
  vertex_color.z = 0.5 + 0.5 * fract(sin(dot(modelpos.xz, vec2(12.9898, 78.233)))* 43758.5453);
}