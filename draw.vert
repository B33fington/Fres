#version 330

in vec2 in_Position;
in vec2 indices;

uniform sampler2D texUnit;


out vec2 outTexCoord;
out vec4 color;

float size = 512.0f;

void main(void){
	outTexCoord = (indices/size)*2.0 - vec2(1.0, 1.0);
	gl_Position = vec4(texture(texUnit, outTexCoord).rg*2.0 - vec2(1.0,1.0), 0.0, 1.0);
}
