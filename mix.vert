#version 330

in vec3 in_Position;

uniform sampler2D partTex;
uniform sampler2D colTex;

out vec2 outTexCoord;

void main(void){
	outTexCoord = in_Position.xy/2.0 + vec2(0.5,0.5);
	gl_Position = vec4(in_Position, 1.0);
}
