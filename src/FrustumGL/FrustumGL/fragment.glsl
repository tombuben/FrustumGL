#version 330 core
in vec3 modelpos;
in vec3 vertex_color;
out vec3 color;
void main(){
  color = vec3(1,0,0);
  //color = modelpos;
  color = vertex_color;
}
