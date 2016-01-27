#version 330

in vec3 in_Position;

uniform sampler2D texUnit;
uniform sampler2D texUnit2;

void main(void){
	gl_Position = vec4(in_Position, 1.0);
}
