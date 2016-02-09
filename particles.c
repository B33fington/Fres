	// gcc particles.c utils/*.c -lGL -lX11 -lm -o particles

	#include <stdlib.h>

	#include <stdio.h>
	#include <math.h>
	#include <stdlib.h>
	#include <time.h> 

	#include <stdio.h>
	#include <GL/gl.h>
	#include "utils/MicroGlut.h"
	//#include "utils/VectorUtils3.h"
	#include "utils/GL_utilities.h"

	#define W 512
	#define H 512
	#define nD 0.7071

	//Global variables
	GLuint drawShader, updateShader, triVertArray, triVertBuffer, indexArray, indexBuffer, tex0, tex1, tex2, fbo0, fbo1, fbo2, colTex, mixShader, texRand;
	int swapper = 0;
	int count = 0;

	int dT, odT;

	GLfloat particles[W][H][4] = {0.0f};
	GLfloat collisions[W][H][4] = {0.0f};
	GLfloat randoms[W][H][4] = {0.0f};

	GLfloat indices[W*H*2];

	//texture to show
	GLfloat texVerts[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f
	};

	void getDeltaTime(){
	dT = glutGet(GLUT_ELAPSED_TIME);
	dT = dT-odT;
	odT = odT + dT;
	}

	createObstacle(int i, int j, int posX, int posY){
		if(sqrt(pow(i-posX,2) + pow(j-posY,2)) < 64)
					collisions[i][j][0] = 1.0f;
				
	}
		
	void calculateNormals(){
		int i,j,step,y1, y2;
		step = 1;		
		for(i=1; i<H-1; i++){
			for(j=1; j<W-1; j++){
				y1=y2=i;
				
				if(collisions[y1][j][0] == 1){
				if(collisions[y1+1][j][0]==0 || collisions[y1][j+1][0] || collisions[y1][j-1][0]){
				if(collisions[y1][j-1][0] == 0){						
					while(collisions[y1-1][j-1][0] == 0 && y1 > 0){
						y1=y1-1;				
					}
				}else{
					while(collisions[y1+1][j-1][0] == 1 && y1 < H){
						y1=y1+1;				
					}
				}
				
				if(collisions[y2][j+1][0] == 0){						
					while(collisions[y2-step][j+1][0] == 0 && y2 > 0 && y2 < H){
						y2=y2-step;				
					}
				}else{
					while(collisions[y2+step][j+1][0] == 1 && y2 > 0 && y2 < H){
						y2=y2+step;				
					}
				}
				}
				}
				
				collisions[i][j][2] = -(y2-y1);
				collisions[i][j][3] = 2;		
			}
		}
	}

	void initScene(){
	int i, j, k, l;
	int r;
	float dx, dy;
	l = 0;
	srand(time(NULL));
	for(i = 0; i < H; ++i){
		for(j = 0; j < W; ++j){
		    
				randoms[i][j][0] = (rand()/(float)RAND_MAX)*2.0 - 1.0;
				randoms[i][j][1] = 0.0f;
				randoms[i][j][2] = 0.001f*((rand()/(float)RAND_MAX)*2.0-1.0);
				randoms[i][j][3] = 0.001f*((rand()/(float)RAND_MAX)*2.0-1.0);

		    r = 1;//rand()%5;

		    //creation of the particles
		    if(r == 1){ 
		        particles[i][j][0] = (float)i/(float)W*2.0 - 1.0;
		        particles[i][j][1] = (float)j/(float)H*1.9 - 1.0;
		        particles[i][j][2] = ((rand()/(float)RAND_MAX)*2.0 - 1.0)*0.001f;
		        particles[i][j][3] = ((rand()/(float)RAND_MAX)*2.0 - 1.0)*0.001f;
		    }else{
						particles[i][j][1] = 10.0f;
		        particles[i][j][2] = 0.0f;
		        particles[i][j][3] = 0.0f;
		    }

		    //creation of the collision texture
				createObstacle(i, j, 256, 256);
				createObstacle(i, j, 170, 150);
				createObstacle(i, j, 170, 362);			

		    indices[l] = (float)i;
		    indices[l+1] = (float)j;
		    l += 2;
		}
		calculateNormals();
	}
	}

	

	void init(void){
	dumpInfo();
	initScene();

	drawShader = loadShaders("draw.vert", "draw.frag");
	updateShader = loadShaders("update.vert", "update.frag");
	mixShader = loadShaders("mix.vert", "mix.frag");

	//Generera arrayer. Måste tydligen göra båda dessa innan vi genererar buffrar!?
	glGenVertexArrays(1, &triVertArray);
	glGenVertexArrays(1, &indexArray);

	//start: to send the position to draw.vert//
	glBindVertexArray(triVertArray);
	glGenBuffers(1, &triVertBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, triVertBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texVerts), texVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(updateShader, "in_Position"), 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), 0);


	//start: send the indices to the draw.vert//
	glBindVertexArray(indexArray);
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(drawShader, "indices"), 2, GL_FLOAT, GL_FALSE, 2*sizeof(GL_FLOAT), 0);

	//tex 1 and fbo object 1
	glGenTextures(1, &tex0);
	glActiveTexture(tex0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, W, H, 0, GL_RGBA, GL_FLOAT, particles);

	glGenFramebuffers(1, &fbo0);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindVertexArray(0);
	//glBindVertexArray();


	//tex 2 and fbo object 2
	glGenTextures(1, &tex1);
	glActiveTexture(tex1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, W, H, 0, GL_RGBA, GL_FLOAT, particles);

	glGenFramebuffers(1, &fbo1);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex1, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);    

	  

	//collision texture//
	glGenTextures(1, &colTex);
	glActiveTexture(colTex);
	glBindTexture(GL_TEXTURE_2D, colTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, W, H, 0, GL_RGBA, GL_FLOAT, collisions);

	//tex 3 and fbo object 3
	glGenTextures(1, &tex2);
	glActiveTexture(tex2);
	glBindTexture(GL_TEXTURE_2D, tex2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, W, H, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenFramebuffers(1, &fbo2);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex2, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);  

	glGenTextures(1, &texRand);
	glActiveTexture(texRand);
	glBindTexture(GL_TEXTURE_2D, texRand);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, W, H, 0, GL_RGBA, GL_FLOAT, randoms);

	}

void display(void){

	getDeltaTime();
	
	

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	
	if(swapper == 0){
		glBindVertexArray(triVertArray);
		glUseProgram(updateShader);
		
		glUniform1i(glGetUniformLocation(updateShader, "random"), (int)((float)rand()/RAND_MAX*1000));
	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex0);    
		glUniform1i(glGetUniformLocation(updateShader, "partTex"), 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, colTex);
		glUniform1i(glGetUniformLocation(updateShader, "colTex"), 2);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, texRand);
		glUniform1i(glGetUniformLocation(updateShader, "randomTex"), 4);

		

		glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindVertexArray(indexArray);
		glUseProgram(drawShader); 
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);

		//glUniform1i(glGetUniformLocation(drawShader, "partTex"), 1);
		swapper = 1;
	} else{
		glBindVertexArray(triVertArray);
		glUseProgram(updateShader);

		glUniform1i(glGetUniformLocation(updateShader, "random"), (int)((float)rand()/RAND_MAX*1000));	

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);
		glUniform1i(glGetUniformLocation(updateShader, "partTex"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, colTex);
		glUniform1i(glGetUniformLocation(updateShader, "colTex"), 2);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, texRand);
		glUniform1i(glGetUniformLocation(updateShader, "randomTex"), 4);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindVertexArray(indexArray);
		glUseProgram(drawShader); 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex0);

		swapper = 0;
	}

	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendEquation( GL_FUNC_ADD );
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_POINTS, 0, W*H);

	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindVertexArray(triVertArray);
	glUseProgram(mixShader);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex2);
	glUniform1i(glGetUniformLocation(mixShader, "partTex"), 3);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, colTex);
	glUniform1i(glGetUniformLocation(mixShader, "colTex"), 2);
	
	

	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glFlush();
	glutSwapBuffers();
}

	// This function is called whenever the computer is idle
	// As soon as the machine is idle, ask GLUT to trigger rendering of a new
	// frame
	void idle()
	{
	glutPostRedisplay();
	}

	int main(int argc, char** argv)
	{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(100, 100);
	glutInitContextVersion(3, 2);
	glutCreateWindow("toabesök efter piri piri");

	glutDisplayFunc(display);
	glutIdleFunc(idle);

	init();

	glutMainLoop();
	printf("%i \n", dT);
	exit(0);
	}



