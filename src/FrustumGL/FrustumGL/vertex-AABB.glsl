#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
uniform mat4 MVP;

out vec3 modelpos;
void main(){
  gl_Position = MVP * vec4(vertexPosition_modelspace,1);
  modelpos.xyz = vertexPosition_modelspace;
}