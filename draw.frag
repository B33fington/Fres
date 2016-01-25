#version 330

in vec2 outTexCoord;
//in vec4 color;
uniform sampler2D texUnit;
out vec4 outColor;

void main(void){
		outColor = vec4(1.0,1.0,0.0,1.0);
}
