#version 330

out vec4 color;

uniform sampler2D texUnit;
uniform sampler2D texUnit2;

vec4 texInfo;
vec4 colTest;
int x;
int y;
float gravity = -0.00000025f;

void main(void){
	texInfo = texelFetch(texUnit, ivec2(gl_FragCoord.xy), 0);

	if(texInfo.g < -1.0f){
		texInfo.g = 1.0f;
		texInfo.a = 0.0f;
	}
	
	//colTest = texture(texUnit2, vec2(texInfo.rg));

	//if(colTest.r == 1.0f){
		//texInfo.b += 0.1;
		//texInfo.a = texInfo.a;
	//}

	texInfo.a = texInfo.a + gravity;

	color = vec4(texInfo.r + texInfo.b, texInfo.g + texInfo.a, texInfo.b, texInfo.a);
}
