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
GLuint drawShader, vertArray, vertBuffer, indexArray, indexBuffer, tex;
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
    //srand(time(NULL));
    int i, j, k, l;
    l = 0;
    for(i = 0; i < W; ++i){
        for(j = 0; j < H; ++j){
            particles[i][j][0] = (float)i/(float)W;
            particles[i][j][1] = (float)j/(float)H;
            particles[i][j][2] = 0.0f;
            particles[i][j][3] = 1.0f;

            indices[l] = (float)i;
            indices[l+1] = (float)j;
            particleVerts[l] = 0.0f;
            particleVerts[l+1] = 0.0f;
            l += 2;
        }
    }
    //printf("%f, %f\n", indices[200], indices[201]);
}

void init(void){
    dumpInfo();
    initParticles();

    drawShader = loadShaders("draw.vert", "draw.frag");
		//Generera arrayer. Måste tydligen göra båda dessa innan vi genererar buffrar!?    
		glGenVertexArrays(1, &vertArray);
    glBindVertexArray(vertArray);
    glGenVertexArrays(1, &indexArray);
    glBindVertexArray(indexArray);

		
    // start: to send the position to the vert shader //
		glGenBuffers(1, &vertBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particleVerts), particleVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(glGetAttribLocation(drawShader, "in_Position"), 2, GL_FLOAT, GL_FALSE, 2*sizeof(GL_FLOAT), 0);
    // end //

    //start: send the indices to the vert shader//
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(glGetAttribLocation(drawShader, "indices"), 2, GL_FLOAT, GL_FALSE, 2*sizeof(GL_FLOAT), 0);
    //


    glGenTextures(1, &tex);
    glActiveTexture(tex); //*
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, W, H, 0, GL_RGBA, GL_FLOAT, particles);
    glUseProgram(drawShader);//*
    glUniform1i(glGetUniformLocation(drawShader, "texUnit"), 0);
    //-------Draw shaders-----------//


}

void display(void){
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(drawShader);
    glBindVertexArray(vertArray);
    glBindVertexArray(indexArray);
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
