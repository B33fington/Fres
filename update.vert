#version 330

in vec3 in_Position;

uniform int random;


void main(void){
	gl_Position = vec4(in_Position, 1.0);
}
