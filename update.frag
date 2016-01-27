#version 330

out vec4 color;

uniform sampler2D texUnit;
vec4 texInfo;
float gravity = -0.0000005f;

void main(void){
	texInfo = texelFetch(texUnit, ivec2(gl_FragCoord.xy), 0);
	texInfo.a = texInfo.a + gravity;
	color = vec4(texInfo.r + texInfo.b, texInfo.g + texInfo.a, texInfo.b, texInfo.a);
}
