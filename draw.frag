#version 330

in vec2 outTexCoord;
in vec4 passColor;
uniform sampler2D texUnit;
out vec4 outColor;

void main(void){
		outColor = passColor;
}
