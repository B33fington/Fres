#version 330

in vec2 indices;
uniform sampler2D partTex;

out vec4 outColor;
in vec2 speed;
float factor; 

void main(void){
		factor = 60.0f*sqrt(pow(speed.x,2) + pow(speed.y,2));
		if(factor > 1.0)
			factor = 1.0;
		outColor = vec4(1.0, 1.0-factor, 0.0, 0.05);
}
