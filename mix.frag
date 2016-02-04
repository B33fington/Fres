#version 330 

in vec2 outTexCoord;

uniform sampler2D partTex;
uniform sampler2D colTex;

out vec4 outColor;


void main(void){
	vec4 tempColor = vec4(0,0,0,1);
	if(texture(colTex, outTexCoord).r == 1)
		tempColor = vec4(1,1,1,1);
	else
		tempColor = texture(partTex, outTexCoord);
	
	outColor = tempColor;// + texture(partTex, outTexCoord);
}
