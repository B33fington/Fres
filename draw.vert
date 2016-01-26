#version 330

in vec2 in_Position;
in vec2 indices;

uniform sampler2D texUnit;

out vec2 outTexCoord;

float size = 512.0f;
vec2 windowCoords;

void main(void){
	outTexCoord = (indices/size);
	windowCoords = texture(texUnit, outTexCoord).rg*2.0 - vec2(1.0,1.0);	

	//sets the GL position according to the particle pos. stored in r & g channels in the texture
	gl_Position = vec4(windowCoords, 0.0, 1.0);
}
