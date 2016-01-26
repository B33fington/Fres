#version 330
in vec2 indices;

uniform sampler2D texUnit;

float size = 512.0f;
vec4 texInfo;
void main(void){

	texInfo = texelFetch(texUnit, ivec2(indices), 0);
	gl_Position = vec4(texInfo.xy, 0.0, 1.0);
}
