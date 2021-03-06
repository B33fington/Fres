#version 330
in vec2 indices;

uniform sampler2D partTex;

float size = 512.0f;
out vec2 speed;
vec4 texInfo;
void main(void){
	texInfo = texelFetch(partTex, ivec2(indices), 0);
	gl_PointSize = 5.0;
	speed = texInfo.ba;
	gl_Position = vec4(texInfo.xy, 0.0, 1.0);
}
