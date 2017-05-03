#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include "md2.h"

#define DELTA_ANGLE 1.0f

int screenwidth, screenheight;
float x_angle, y_angle;
static GLuint* pIndices = 0;
static GLfloat* pVertexdata;
static GLvoid* pPixels;//pixel data
float height_map[128][128];//height map
static GLsizei count;
static GLuint terrainTexture; 
float width, height;
bool show_frame, show_texture, near_view;

GLuint Decal_Texture;
md2_model_t* md2_model;
md2_model_t* md2_model_2;
float model_center[3], model_radius;
float model_center_2[3], model_radius_2;
int current_frame, current_subframe, MaxFrame=39, MinFrame=1;

void InitializeGlut(int *argc, char *argv[]);
void Display(void);
void Keyboard(unsigned char key, int x, int y);
bool loadTexture(char *TexName, GLuint TexHandle);
void loadModel();
