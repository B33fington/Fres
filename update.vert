#version 330

in vec3 in_Position;

uniform sampler2D texUnit;

void main(void){
	gl_Position = vec4(in_Position, 1.0);
}
