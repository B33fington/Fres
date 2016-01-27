#version 330

out vec4 color;

uniform sampler2D partTex;
uniform sampler2D colTex;

vec4 texInfo;
vec4 colTest;
vec2 reflect;
int x;
int y;
float gravity = -0.00000025f;

void main(void){
	texInfo = texelFetch(partTex, ivec2(gl_FragCoord.xy), 0);

	if(texInfo.g < -1.0f){
		texInfo.g = 1.0f;
		texInfo.a = 0.0f;
	}
	
	colTest = texture(colTex, vec2(texInfo.rg/2.0+vec2(0.5,0.5)));

	if(colTest.r == 1.0f){
		reflect = 2.0 * dot(colTest.ba,texInfo.ba) * colTest.ba - texInfo.ba; 
		texInfo.ba = 0.5*reflect;
	}
	texInfo.a = texInfo.a + gravity;

	color = vec4(texInfo.r + texInfo.b, texInfo.g + texInfo.a, texInfo.b, texInfo.a);
}
