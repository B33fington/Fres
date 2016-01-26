#version 330

out vec4 color;

uniform sampler2D texUnit;
vec4 texInfo;
float gravity;

void main(void){
	gravity = -0.001f;
	texInfo = texelFetch(texUnit, ivec2(gl_FragCoord.xy), 0);
	color = vec4(texInfo.r + texInfo.b, texInfo.g + texInfo.a, texInfo.b, texInfo.a);
}
