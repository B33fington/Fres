#version 330

out vec4 color;

uniform int random;
uniform sampler2D partTex;
uniform sampler2D colTex;
uniform sampler2D randomTex;


vec4 texInfo;
vec4 colTest;
vec2 reflect;		
int x;
int y;
float gravity = -0.0001f;
float speed;
float epsilon = 0.2;
float mass = 10.0;
void main(void){
	texInfo = texelFetch(partTex, ivec2(gl_FragCoord.xy), 0);
	reflect = vec2(0,0);

	if(texInfo.g <= -1.0f || texInfo.r < -1.0 || texInfo.r > 1.0){
		texInfo.r = texelFetch(randomTex, ivec2(gl_FragCoord.xy), 0).r;
		texInfo.g = 1.0f; 		
		texInfo.b = texelFetch(randomTex, ivec2(gl_FragCoord.xy), 0).b;
		texInfo.a = texelFetch(randomTex, ivec2(gl_FragCoord.xy), 0).a;
		
	}
	
	colTest = texture(colTex, vec2(texInfo.rg/2.0+vec2(0.5,0.5)));

	//jr = ( -(1+e)vr . n ) / (inv(m1)) 
	//v' = v - jr/m1 * n
	
	if(abs(texInfo.g - 10.0f) > 0.001f){
	if(abs(colTest.r - 1.0f) < 0.001f  && texInfo.r > -1.0 && texInfo.r < 1.0){
		
		float impulse = (-(1+epsilon)*dot(texInfo.ba, normalize(colTest.ba))) / (1.0/mass);
		texInfo.ba = texInfo.ba + (impulse/mass) * normalize(colTest.ba);

	}
	texInfo.a = texInfo.a + gravity;
	}
	color = vec4(texInfo.r + texInfo.b, texInfo.g + texInfo.a, texInfo.b, texInfo.a); 
}

/*speed = length(texInfo.ba);
		reflect = 2.0 * dot(normalize(colTest.ba),texInfo.ba) * normalize(colTest.ba) - texInfo.ba; 
		texInfo.ba = 0.2*speed*reflect;*/
