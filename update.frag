#version 330
in vec2 outTexCoord;
out vec4 color;

uniform sampler2D texUnit;
float px, py, dx, dy;

void main(void){
	px = texture(texUnit, outTexCoord).r;
	py = texture(texUnit, outTexCoord).g;
	dx = texture(texUnit, outTexCoord).b;
	dy = texture(texUnit, outTexCoord).a;

	color = vec4(px+dx, py+dy, dx, dy);
}
