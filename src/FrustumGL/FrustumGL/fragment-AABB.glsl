#version 330 core
in vec3 modelpos;
out vec4 color;
void main(){
  color = vec4(1,0,0,0.5);
  color = vec4(abs(modelpos), gl_FragCoord.z / 10.0);
}
