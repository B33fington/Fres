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

//Global variables
GLuint drawShader, updateShader, vertArray, vertBuffer, indexArray, indexBuffer, tex0, tex1, fbo0, fbo1;
int swapper = 0;

GLfloat particles[W][H][4];

GLfloat particleVerts[W*H*2];
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

void initParticles(){
    int i, j, k, l;
    int r;
    float dx, dy;
    l = 0;
    srand(time(NULL));
    for(i = 0; i < W; ++i){
        for(j = 0; j < H; ++j){
            
            r = (int)round(rand()/(float)RAND_MAX);
           
            particles[i][j][0] = 0.0f;
            particles[i][j][1] = 0.0f;
            particles[i][j][2] = 0.01f;
            particles[i][j][3] = 0.0f;

            if(r == 1){
                particles[i][j][0] = (float)i/(float)W;
                particles[i][j][1] = (float)j/(float)H;
            }

            indices[l] = (float)i;
            indices[l+1] = (float)j;
            particleVerts[l] = 0.0f;
            particleVerts[l+1] = 0.0f;
            l += 2;
        }
    }
}

void init(void){
    dumpInfo();
    initParticles();

    drawShader = loadShaders("draw.vert", "draw.frag");
    updateShader = loadShaders("update.vert", "update.frag");

	//Generera arrayer. Måste tydligen göra båda dessa innan vi genererar buffrar!?    
	glGenVertexArrays(1, &vertArray);
    glBindVertexArray(vertArray);
    glGenVertexArrays(1, &indexArray);
    glBindVertexArray(indexArray);

		
    // start: to send the position to draw.vert//
	glGenBuffers(1, &vertBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particleVerts), particleVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(glGetAttribLocation(drawShader, "in_Position"), 2, GL_FLOAT, GL_FALSE, 2*sizeof(GL_FLOAT), 0);
    glVertexAttribPointer(glGetAttribLocation(updateShader, "in_Position"), 2, GL_FLOAT, GL_FALSE, 2*sizeof(GL_FLOAT), 0);

    //start: send the indices to the draw.vert//
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(glGetAttribLocation(drawShader, "indices"), 2, GL_FLOAT, GL_FALSE, 2*sizeof(GL_FLOAT), 0);
    glVertexAttribPointer(glGetAttribLocation(updateShader, "indices"), 2, GL_FLOAT, GL_FALSE, 2*sizeof(GL_FLOAT), 0);

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
    //-------Draw shaders-----------//
}

void display(void){
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vertArray);
    glBindVertexArray(indexArray);

    if(swapper == 0){

        glUseProgram(updateShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0);
        glUniform1i(glGetUniformLocation(updateShader, "texUnit"), 0);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
        glDrawArrays(GL_POINTS, 0, W*H);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glUseProgram(drawShader); 
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1);

        glUniform1i(glGetUniformLocation(drawShader, "texUnit"), 1);
        swapper = 1;
        printf("%s\n", "0");

    }else{

        glUseProgram(updateShader);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glUniform1i(glGetUniformLocation(updateShader, "texUnit"), 1);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo0);
        glDrawArrays(GL_POINTS, 0, W*H);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glUseProgram(drawShader); 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0);

        glUniform1i(glGetUniformLocation(drawShader, "texUnit"), 0);
        swapper = 0;
        printf("%s\n", "1");
    }

    glDrawArrays(GL_POINTS, 0, W*H);
    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(100, 100);
    glutInitContextVersion(3, 2);
    glutCreateWindow("Rakt i Prutten");
    
    glutDisplayFunc(display);

    init();

    glutMainLoop();
    
    return 0;
}
