// gcc particles.c utils/*.c -lGL -lX11 -lm -o particles

#include <stdlib.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h> 

#include <stdio.h>
#include <GL/gl.h>
#include "utils/MicroGlut.h"
#include "utils/VectorUtils3.h"
#include "utils/GL_utilities.h"

#define W 512
#define H 512
#define nD 0.7071

//Global variables
GLuint drawShader, updateShader, triVertArray, triVertBuffer, indexArray, indexBuffer, tex0, tex1, fbo0, fbo1, colTex;
int swapper = 0;
int count = 0;

GLfloat particles[W][H][4] = {0.0f};
GLfloat collisions[W][H][4] = {0.0f};

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

void initScene(){
    int i, j, k, l;
    int r;
    float dx, dy;
    l = 0;
    srand(time(NULL));
    for(i = 0; i < W; ++i){
        for(j = 0; j < H; ++j){
            
            r = rand()%2;

            //creation of the particles
            if(r == 1){ 
                particles[i][j][0] = (float)i/(float)W*2.0 - 1.0;
                particles[i][j][1] = 1.0f;//(float)j/(float)H*2.0 - 1.0;
                particles[i][j][2] = ((rand()/(float)RAND_MAX)*2.0 - 1.0)*0.00001f;
                particles[i][j][3] = ((rand()/(float)RAND_MAX)*2.0 - 1.0)*0.00001f;
            } else{
                particles[i][j][0] = -1.0f;
                particles[i][j][1] = -1.0f;
            }

            //creation of the collision texture
            if(i <= j){
                collisions[i][j][0] = 1.0f;
                collisions[i][j][2] = nD;
                collisions[i][j][3] = nD;
            }

            indices[l] = (float)i;
            indices[l+1] = (float)j;
            l += 2;
        }
    }
}

void init(void){
    dumpInfo();
    initScene();

    drawShader = loadShaders("draw.vert", "draw.frag");
    updateShader = loadShaders("update.vert", "update.frag");

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

}

void display(void){
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

    if(swapper == 0){
        glBindVertexArray(triVertArray);
        glUseProgram(updateShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0);    
        glUniform1i(glGetUniformLocation(updateShader, "partTex"), 0);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, colTex);
        glUniform1i(glGetUniformLocation(updateShader, "colTex"), 2);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
				
        glBindVertexArray(indexArray);
        glUseProgram(drawShader); 
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1);

        glUniform1i(glGetUniformLocation(drawShader, "partTex"), 1);
        swapper = 1;
    } else{
        glBindVertexArray(triVertArray);
        glUseProgram(updateShader);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glUniform1i(glGetUniformLocation(updateShader, "partTex"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, colTex);
        glUniform1i(glGetUniformLocation(updateShader, "colTex"), 2);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
        glBindVertexArray(indexArray);
		glUseProgram(drawShader); 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0);

        glUniform1i(glGetUniformLocation(drawShader, "colTex"), 0);
        swapper = 0;
    }
    
    /*
    glEnable(GL_BLEND);
    glBlendEquation( GL_FUNC_ADD );
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    */
		
    glDrawArrays(GL_POINTS, 0, W*H);
    glBindTexture(GL_TEXTURE_2D, 0);

    //glDisable(GL_BLEND);
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
  glutCreateWindow("We'll have all the details! Plus, Chopper Dave! HEYOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
  
  glutDisplayFunc(display);
	glutIdleFunc(idle);

    init();

    glutMainLoop();
    
    exit(0);
}



