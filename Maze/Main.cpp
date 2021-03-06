#include "stdafx.h"
#include "tag.h"
#include "Testmd2.h"
#include <GL/glut.h>
#include <GL/glaux.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <random>

//自己编写的
#include "seekRoad.h"

#define WALL 1
#define GROUND 2
#define MAZE_WALK 1
#define MAZE_EDIT 2
#define FIRST_PERSON 1
#define TRIRD_PERSON 3
//全局变量
const int textnum = 3;
GLuint texture[textnum];//存储纹理数据
float wall[8][3];//存放画墙的正方体

int window_width = 640;
int window_height = 640;
int crt_window = MAZE_WALK; //1为迷宫窗口，2为绘制迷宫窗口
int draw_which = 0; //WALL =1 GROUND =2
int maze_size = 20;
int view = TRIRD_PERSON; //默认第三人称
double crt_pos_x, crt_pos_z, save_pos_x, save_pos_z, look_x, look_z, eye_x, eye_y=0.5, eye_z;
double angle, distant = 5, angleM = 90;//视角，视点离模型的位置,模型
int bonus_r, bonus_c;
double bili = 4;//第三视角事件体与屏幕坐标比例
double box_r = 7;//墙体大小
int score;
SeekRoad seek;
#define MAX_CHAR 128
static GLuint lists;
int maze[105][105]= {
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1 },
	{ 1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1 },
	{ 1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1 },
	{ 1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,0,1,1 },
	{ 1,0,1,1,1,1,1,1,0,0,0,0,0,1,1,0,0,0,1,1 },
	{ 1,0,1,1,1,1,1,1,0,0,0,0,0,1,1,0,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,1 },
	{ 1,0,1,1,1,1,1,1,0,0,0,0,0,1,1,0,1,0,1,1 },
	{ 1,0,1,1,1,1,1,1,0,0,0,0,0,1,0,0,1,0,1,1 },
	{ 1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,0,1,1 },
	{ 1,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,0,1,1 },
	{ 1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,1,1 },
	{ 1,1,1,0,1,1,1,0,0,1,1,1,1,0,0,0,0,0,1,1 },
	{ 1,0,0,0,1,0,1,1,0,1,1,0,0,0,1,0,1,0,0,1 },
	{ 1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }
};
int new_maze[105][105];//maze:当前迷宫 new_maze:当前编辑的迷宫
int route[10000],route_size;
int space_r[10100], space_c[10100];
int space_count = 0;

void display();
void switchWindow(int window);

void setLookEye() {
	//将视角与当前位置对应
	look_x = crt_pos_x + distant*cos(angle);
	look_z = crt_pos_z - distant*sin(angle);
	eye_x = crt_pos_x - distant*cos(angle);
	eye_z = crt_pos_z + distant*sin(angle);
}

void generateBasicRandom() {
	for (int i = 0; i < maze_size; i++) {
		for (int j = 0; j < maze_size; j++) {
			if (maze[i][j] == 0) {
				space_r[space_count] = i; space_c[space_count] = j;
				space_count++;
			}
		}
	}
}

void generateNewCR(int &c, int &r) {
	if (space_count == 0)return;
	int num = rand() % space_count;
	c = space_c[num]; r = space_r[num];
}

void checkBonus() {
	int maze_c = (int)((crt_pos_x + box_r / 2) / box_r);//将屏幕坐标对应为数组下标
	int maze_r = (int)((crt_pos_z + box_r / 2) / box_r);
	if (maze_c == bonus_c && maze_r == bonus_r) {
		generateNewCR(bonus_c, bonus_r);
		score++;
	}
	
}

void moveTo(int x, int z) {
	int crt_r = (int)((crt_pos_z + box_r / 2) / box_r);
	int crt_c = (int)((crt_pos_x + box_r / 2) / box_r);
	printf("crt: r:%d c:%d Moveto r:%d c:%d\n",crt_r ,crt_c ,z, x);
	route_size = seek.seekroad(maze, maze_size, maze_size, route, crt_c, crt_r, x, z);
	if (route_size == -1) {
		printf("不可到达\n");
		return;
	}
	for (int i = 0; i < route_size; i++) {
		for (int j = 0; j < box_r; j++) {
			crt_pos_z += seek.mv[route[i]][0];
			crt_pos_x += seek.mv[route[i]][1]; 
			setLookEye();
			display();
		}
		checkBonus();
	}

}

void menu(int id) {
	switch (id) {
	case 1:
		generateNewCR(bonus_c, bonus_r);
		break;
	case 2:
		view = FIRST_PERSON;
		glutPostRedisplay();
		break;
	case 3:
		view = TRIRD_PERSON;
		glutPostRedisplay();
		break;
	case 4:
		save_pos_x = crt_pos_x;
		save_pos_z = crt_pos_z;
		glutPostRedisplay();
		break;
	case 5:
		//if (view == TRIRD_PERSON)
			moveTo((int)((save_pos_x + box_r / 2) / box_r), (int)((save_pos_z + box_r / 2) / box_r));
		setLookEye();
		glutPostRedisplay();
		break;
	case 6:
		switchWindow(MAZE_EDIT);
		break;
	}
}

void newMenu(int id){
	switch (id){
	case 1:
		draw_which = WALL;
		break;
	case 2:
		draw_which = GROUND;
		break;
	case 3://保存
		memcpy(maze, new_maze, sizeof maze); 
		generateBasicRandom();
		break;
	case 4://退出
		switchWindow(MAZE_WALK);
		break;
	case 5://清空
		memset(new_maze, 0, sizeof new_maze);
		for (int i = 0; i<20; i++)
			new_maze[0][i] = new_maze[maze_size -1][i] = new_maze[i][0] = new_maze[i][maze_size -1] = 1;
		new_maze[1][0] = 0;//留入口
		glutPostRedisplay();
		break;
	}
}

void createMyMenu() {
	if (crt_window == 1) {
		glutCreateMenu(menu);
		glutAddMenuEntry("换一个bonus", 1);
		glutAddMenuEntry("第一视角", 2);
		glutAddMenuEntry("第三视角", 3);
		glutAddMenuEntry("保存当前位置", 4);
		glutAddMenuEntry("返回保存位置", 5);
		glutAddMenuEntry("设置迷宫", 6);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
	}
	else if(crt_window == 2){
		glutCreateMenu(newMenu);
		glutAddMenuEntry("绘制墙", 1);
		glutAddMenuEntry("绘制草地", 2);
		glutAddMenuEntry("保存", 3);
		glutAddMenuEntry("返回迷宫场景", 4);
		glutAddMenuEntry("清空", 5);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
	}
}

void switchWindow(int window) {
	if(window==MAZE_EDIT)
		memcpy(new_maze, maze, sizeof maze);
	crt_window = window;
	createMyMenu();
}

void myMouseFunc(int button,int state,int x,int y){
	x = x / bili - 10;
	y = y / bili - 10;//将屏幕坐标与事件体对应

	int new_maze_x = (int)((x + box_r/2) / box_r);//将屏幕坐标对应为数组下标
	int new_maze_z = (int)((y + box_r/2) / box_r);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && view == TRIRD_PERSON && crt_window == MAZE_WALK)
	{
		moveTo(new_maze_x, new_maze_z);
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN  && crt_window == MAZE_EDIT)
	{
		if(draw_which == WALL)
			new_maze[new_maze_z][new_maze_x] = 1;
		if(draw_which == GROUND)
			new_maze[new_maze_z][new_maze_x] = 0;
	}
	if (button == GLUT_WHEEL_UP) {
		distant--;
		printf("distant %lf\n", distant);
		setLookEye();
		display();
	}
	if (button == GLUT_WHEEL_DOWN) {
		distant++;
		printf("distant %lf\n", distant);
		setLookEye();
		display();
	}

}

void inputKeyFunc(int key, int x, int y) {
	if (crt_window == MAZE_WALK) {
		int oldpos_x = crt_pos_x;
		int oldpos_z = crt_pos_z;
		if (view == FIRST_PERSON) {
			switch (key) {
			case GLUT_KEY_LEFT:
				angle += 0.01f*5;
				angleM += 0.573*5;
				break;
			case GLUT_KEY_RIGHT:
				angle -= 0.01f*5;
				angleM -= 0.573*5;
				break;
			case GLUT_KEY_UP:
				crt_pos_x += cos(angle) * 1;
				crt_pos_z += -sin(angle) * 1;
				break;
			case GLUT_KEY_DOWN:
				crt_pos_x -= cos(angle) * 1;
				crt_pos_z -= -sin(angle) * 1;
				break;
			}
		}
		if (view == TRIRD_PERSON) {
			switch (key) {
			case GLUT_KEY_LEFT:
				crt_pos_x--;
				break;
			case GLUT_KEY_RIGHT:
				crt_pos_x++;
				break;
			case GLUT_KEY_UP:
				crt_pos_z--;
				break;
			case GLUT_KEY_DOWN:
				crt_pos_z++;
				break;
			}
		}
		std::cout << "crt_pos_x  " << crt_pos_x << "  crt_pos_z  " << crt_pos_z << std::endl;

		//保存当前位置到位置迷宫中，以重新绘制小地图
		//savePosMaze();

		//碰撞检测
		int maze_c = (int)((crt_pos_x + box_r / 2) / box_r);//将屏幕坐标对应为数组下标
		int maze_r = (int)((crt_pos_z + box_r / 2) / box_r);
		//墙
		for (int i = maze_c - 1; i <= maze_c + 1; i++) {
			for (int j = maze_r - 1; j <= maze_r + 1; j++) {
				if (maze[j][i] == 1){
					float maze_pos_x = (float)i*box_r;
					float maze_pos_z = (float)j*box_r;
					//cout<< "maze_pos_x  "<<maze_pos_x<<"    maze_pos_z  "<<maze_pos_z<<endl;
					int model_w = 3;
					if (view == 1)model_w = 1;
					else model_w = 3;
					//判断墙的厚度以及替身的宽度与当前位置
					if (crt_pos_x>(maze_pos_x - box_r / 2 - model_w) && crt_pos_x<(maze_pos_x + box_r / 2 + model_w) && 
						crt_pos_z>(maze_pos_z - box_r / 2 - model_w) && crt_pos_z<(maze_pos_z + box_r / 2 + model_w))
					{
						crt_pos_x = oldpos_x;
						crt_pos_z = oldpos_z;
						//std::cout << "墙： " << " maze_pos_x  " << j << "  maze_pos_z  " << i << std::endl;
					}
				}
			}
		}
		//球
		checkBonus();
		setLookEye();
		glutPostRedisplay();
	}
}

void processNormalKeys(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a':
		inputKeyFunc(GLUT_KEY_LEFT, x, y); break;
	case 'd':
		inputKeyFunc(GLUT_KEY_RIGHT, x, y); break;
	case 'w':
		inputKeyFunc(GLUT_KEY_UP, x, y); break;
	case 's':
		inputKeyFunc(GLUT_KEY_DOWN, x, y); break;
	case 'v':
		eye_y++; break; 
	case 'b':
		eye_y--; break;
	default:
		break;
	}

}

AUX_RGBImageRec *LoadBMP(char *Filename) {		// 载入位图图象 		
	FILE *File = NULL;

	if (!Filename) {								//确定给定的文件名有效
		return NULL;							// 如果没提供，返回 NULL   
	}

	File = fopen(Filename, "r");					// 打开文件	

	if (File) {
		fclose(File);							// 关闭文件句柄	
		File = NULL;
		return auxDIBImageLoad(Filename);       // 载入位图并返回指针
	}

	return NULL;								// 如果载入失败，返回 NULL   
}

int loadGLTextures() {        // 根据加载的位图创建纹理
	int Status = TRUE;         // 指示纹理创建是否成功的标志

	AUX_RGBImageRec *TextureImage[textnum];     // 创建一个纹理图像数组，这里指定数组大小为3

	memset(TextureImage, 0, sizeof(void *) * textnum);           // 初始化纹理图像数组，为其分配内存

	char *pictures[] = { // 创建一个位图名称数组，对应3幅位图
		"wall.bmp",
		"ground.bmp",
		"micai.bmp",
	};
	for (int i = 0; i<textnum; i++) { // 遍历位图名称数组，根据位图名称分别生成
		if (TextureImage[i] = LoadBMP(pictures[i])) { // 加载位图i成功，修改状态标志变量Status为TRUE

			glGenTextures(1, &texture[i]);     // 为第i个位图创建纹理
			glBindTexture(GL_TEXTURE_2D, texture[i]); // 将生成的纹理的名称绑定到指定的纹理上
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[i]->sizeX, TextureImage[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[i]->data);//用来指定二维纹理图像
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//实现线形滤波的功能，当纹理映射到图形表面以后，如果因为其它条件的设置导致纹理不能更好地显示的时候，进行过滤，按照指定的方式进行显示，可能会过滤掉显示不正常的纹理像素。
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//线性滤波
		}
		else {
			Status = FALSE;
			puts("BOOM");
		}

		if (TextureImage[i]) {         // 纹理是否存在
			if (TextureImage[i]->data) { // 纹理图像是否存在      
				free(TextureImage[i]->data); // 释放纹理图像占用的内存  
			}
			free(TextureImage[i]); // 释放图像结构       
		}
	}
	return Status;          // 创建纹理并加载，返回成功或者失败的标志Status
}

void makeBox(float x, float y, float z)
{
	wall[0][0] = -x / 2;	wall[0][1] = y / 2;	wall[0][2] = -z / 2;
	wall[1][0] = -x / 2;	wall[1][1] = -y / 2;	wall[1][2] = -z / 2;
	wall[2][0] = x / 2;	wall[2][1] = -y / 2;	wall[2][2] = -z / 2;
	wall[3][0] = x / 2;	wall[3][1] = y / 2;	wall[3][2] = -z / 2;

	wall[4][0] = -x / 2;	wall[4][1] = y / 2;	wall[4][2] = z / 2;
	wall[5][0] = -x / 2;	wall[5][1] = -y / 2;	wall[5][2] = z / 2;
	wall[6][0] = x / 2;	wall[6][1] = -y / 2;	wall[6][2] = z / 2;
	wall[7][0] = x / 2;	wall[7][1] = y / 2;	wall[7][2] = z / 2;

	//glColor3f(1.0f, 0.0f, 0.0f);
	int points[6][4] = { 0,1,2,3,  4,5,6,7,  4,0,3,7,  3,2,6,7,  4,5,1,0,  6,5,1,2 };
	for (int i = 0; i < 6; i++) {
		glBindTexture(GL_TEXTURE_2D, texture[0]); // 选择第一个纹理texture[0]，进行贴纹理
		glBegin(GL_POLYGON);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3fv(wall[points[i][0]]);
		glTexCoord2f(1.0f, 0.0f); glVertex3fv(wall[points[i][1]]);
		glTexCoord2f(1.0f, 1.0f); glVertex3fv(wall[points[i][2]]);
		glTexCoord2f(0.0f, 1.0f); glVertex3fv(wall[points[i][3]]);
		glEnd();
	}
}

void myInit() {
	loadGLTextures();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//glClearDepth(1.0f);//设置深度缓存
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 清除屏幕和深度缓存
													   //glDepthFunc(GL_LEQUAL);//深度小或相等的时候也渲染
													   //glShadeModel(GL_SMOOTH);// 启用阴影平滑
	crt_pos_x = 0; crt_pos_z = box_r;
	bonus_r = 1, bonus_c = 1;
	score = 0;
	generateBasicRandom();
	setLookEye();
	
	GLfloat sun_light_position[] = { 1, 1, 1, 0.0f };
	GLfloat sun_light_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat sun_light_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat sun_light_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position); //指定第0号光源的位置   
	glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient); //GL_AMBIENT表示各种光线照射到该材质上，  
														 //经过很多次反射后最终遗留在环境中的光线强度（颜色）  
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse); //漫反射后~~  
	glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);//镜面反射后~~~  

	glEnable(GL_LIGHT0); //使用第0号光照  
	glEnable(GL_LIGHTING); //在后面的渲染中使用光照  
	glEnable(GL_DEPTH_TEST); //这句是启用深度测试，这样，在后面的物体会被挡着，例如房子后面有棵树，如果不启用深度测试，  
							 //你先画了房子再画树，树会覆盖房子的；但启用深度测试后无论你怎么画，树一定在房子后面（被房子挡着）   

	//glLightModeli(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, TRUE);

	// 如果是第一次调用，执行初始化 为每一个ASCII字符产生一个显示列表
	// 申请MAX_CHAR个连续的显示列表编号
	lists = glGenLists(MAX_CHAR);
	// 把每个字符的绘制命令都装到对应的显示列表中
	wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
}
/*
bool loadTexture(char *TexName, GLuint TexHandle)
{
	TGAImg Img;        // Image loader

					   // Load our Texture
	if (Img.Load(TexName) != IMG_OK)
		return false;

	glBindTexture(GL_TEXTURE_2D, TexHandle); // Set our Tex handle as current

											 // Create the texture
	if (Img.GetBPP() == 24)
		glTexImage2D(GL_TEXTURE_2D, 0, 3, Img.GetWidth(), Img.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, Img.GetImg());
	else if (Img.GetBPP() == 32)
		glTexImage2D(GL_TEXTURE_2D, 0, 4, Img.GetWidth(), Img.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Img.GetImg());
	else
		return false;

	// Specify filtering and edge actions
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	return true;
}
*/
void loadModel() {
	float minmax[6];

	md2_model_2 = md2_readModel("WalkMech.md2");
	md2_getBoundingBox(md2_model_2, minmax);

	model_center_2[0] = (minmax[0] + minmax[1]) / 2.0f;
	model_center_2[1] = (minmax[2] + minmax[3]) / 2.0f;
	model_center_2[2] = (minmax[4] + minmax[5]) / 2.0f;
	model_radius_2 = sqrt((minmax[0] - minmax[1]) * (minmax[0] - minmax[1]) +
		(minmax[2] - minmax[3]) * (minmax[2] - minmax[3]) +
		(minmax[4] - minmax[5]) * (minmax[4] - minmax[5])) / 2.0;

	//loadTexture("GenericMech.tga", Decal_Texture);
}

void drawGround() {

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[1]); // 选择第二个纹理texture[1]，进行贴纹理
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);  glVertex3f(-box_r / 2, -box_r / 2, -box_r / 2);
	glTexCoord2f(1.0f, 1.0f);  glVertex3f(-box_r / 2, -box_r / 2, box_r*maze_size - box_r / 2);
	glTexCoord2f(1.0f, 0.0f);  glVertex3f(box_r*maze_size - box_r / 2, -box_r / 2, box_r*maze_size - box_r / 2);
	glTexCoord2f(0.0f, 0.0f);  glVertex3f(box_r*maze_size - box_r / 2, -box_r / 2, -box_r / 2);
	glEnd();
	glPopMatrix();

}

void addLabel(const char* str) {
	// 调用每个字符对应的显示列表，绘制每个字符
	for (; *str != '\0'; ++str)
		glCallList(lists + *str);
}

void drawSphere() {
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	//画分数球
	GLfloat sun_mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat sun_mat_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat sun_mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//GLfloat sun_mat_emission[] = { 250.0/256, 252.0/256, 173.0/256, 1.0f };
	GLfloat sun_mat_shininess = 128.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, sun_mat_ambient); //定义材料的前面采用 "多次反射"  
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, sun_mat_diffuse); //材料的前面为 漫反射  
	glMaterialfv(GL_FRONT, GL_SPECULAR, sun_mat_specular); //定义材料的前面为 镜面反射  
														   //glMaterialfv(GL_FRONT, GL_EMISSION, sun_mat_emission); //定义材料的前面为 镜面指数  
	glMaterialf(GL_FRONT, GL_SHININESS, sun_mat_shininess); //材料的前面 采用 的颜色  

	
	double bonus_x = bonus_c * 7;
	double bonus_z = bonus_r * 7;
	glColor3f(0.2, 0.2, 0.2);
	glTranslatef(bonus_x, 0, bonus_z);
	glutSolidSphere(3.5, 400, 500);
	glTranslatef(-bonus_x, 0, -bonus_z);
	glColor4f(1, 1, 1, 1);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
}

void drawScore() {
	glDisable(GL_LIGHT0);
	//分数
	glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos3f(120, 10, -5);
	char str[20]("Your Score: ");
	_itoa(score, str + 12, 10);
	addLabel(str);
	glColor4f(1, 1, 1, 1);
	glEnable(GL_LIGHT0);
	//puts("Score");
}

void drawMaze() {
	//画地面
	drawGround();

	//画迷宫
	for (int i = 0; i<maze_size; i++) {
		for (int j = 0; j<maze_size; j++) {
			if (maze[j][i] == 1) {
				glPushMatrix();
				glTranslatef((float)i*box_r, 0.0f, (float)j*box_r);
				makeBox(box_r, box_r, box_r);
				glPopMatrix();
			}
		}
	}
	drawSphere();

}

void display() {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor4f(1, 1, 1, 1);

	if (crt_window == MAZE_WALK) {

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);

		//glBindTexture(GL_TEXTURE_2D, Decal_Texture);
		glBindTexture(GL_TEXTURE_2D, texture[2]);

		glPushMatrix();
		glTranslatef(crt_pos_x, -1.5, crt_pos_z);//模型位置
		glScalef(0.1*view, 0.1*view, 0.1*view);//调整模型大小，避免第三视角时，模型太小，不好观察

		glRotatef(angleM, 0.0, 1.0, 0.0);//模型方向

		md2_drawModel(md2_model_2, current_frame, current_frame + 1, (float)current_subframe / 10);//LOOP_FRAME_NUM);
		glPopMatrix();

		current_subframe++;
		if (current_subframe == 10)//LOOP_FRAME_NUM)
		{
			current_subframe = 0;

			//tris
			if (current_frame < MaxFrame)
				current_frame++;
			else {
				MaxFrame = 39;
				current_frame = MinFrame = 0;
			}
			if (current_frame == md2_model_2->header.numFrames - 1)
				current_frame = 0;

		}

		glutPostRedisplay();
		
		if (view == TRIRD_PERSON) {

			glViewport(0, 0, window_width, window_height);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glOrtho(-10, 150, -150, 10, -10, 10);
			glRotatef(90, 1.0f, 0.0f, 0.0f);
			drawScore();
			drawMaze();
		}
		
		if (view == FIRST_PERSON) {

			//小地图

			glViewport(window_width - 200, window_height - 200, 200, 200);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glOrtho(-10, 150, -150, 10, -10, 10);
			glRotatef(90, 1.0f, 0.0f, 0.0f);

			drawMaze();

			//载入模型


			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, texture[2]);


			//Tris
			glPushMatrix();
			glTranslatef(crt_pos_x, 3, crt_pos_z);//模型位置
			glScalef(0.2, 0.2, 0.2);//调整模型大小

			glRotatef(angleM, 0.0, 1.0, 0.0);//模型方向

			md2_drawModel(md2_model_2, current_frame, current_frame + 1, (float)current_subframe / 10);//LOOP_FRAME_NUM);
			glPopMatrix();



			current_subframe++;
			if (current_subframe == 10)//LOOP_FRAME_NUM)
			{
				current_subframe = 0;

				//tris
				if (current_frame < MaxFrame)
					current_frame++;
				else {
					MaxFrame = 39;
					current_frame = MinFrame = 0;
				}
				if (current_frame == md2_model_2->header.numFrames - 1)
					current_frame = 0;

			}




			//第一视角的大图
			// 切换到投影模式

			glViewport(0, 0, window_width, window_height);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			gluPerspective(30.0, (double)window_width / (double)window_height, 1.0f, 300.0f);//可以控制视野
			gluLookAt(eye_x, eye_y, eye_z, // 视点，为了能看见模型，所以视点是在以模型为中心的圆上。
				look_x, 0.0f, look_z, 0.0f, 1.0f, 0.0f);

			glMatrixMode(GL_MODELVIEW);

			glLoadIdentity();

			drawMaze();

		}
		
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);

		glutSwapBuffers();
	}
	
	if (crt_window == MAZE_EDIT) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glOrtho(-10, 150, -150, 10, -10, 10);
		glRotatef(90, 1.0f, 0.0f, 0.0f);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glColor4f(1, 1, 1, 1);

		drawGround();
		//画迷宫
		for (int i = 0; i<maze_size; i++){
			for (int j = 0; j<maze_size; j++){
				if (new_maze[j][i] == 1)
				{
					glPushMatrix();
					glTranslatef((float)i*box_r, 0.0f, (float)j*box_r);
					makeBox(box_r, box_r, box_r);
					glPopMatrix();
				}
			}
		}

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);

		//绘制网格
		glColor3d(1, 0, 0);
		glBegin(GL_LINES);

		for (int x = -box_r / 2; x <= maze_size*box_r + box_r / 2; x += box_r) {


			glVertex3f(x, 1, -box_r / 2);
			glVertex3f(x, 1, maze_size*box_r - box_r / 2);

		}
		for (int z = -box_r / 2; z <= maze_size*box_r + box_r / 2; z += box_r) {

			glVertex3f(-box_r / 2, 1, z);
			glVertex3f(maze_size*box_r - box_r / 2, 1, z);
		}

		glEnd();

		
		/*	//设置方块线
		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisable(GL_DEPTH_TEST);
		glColor4f(1,0,0,0.4);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		for(int i=0;i<maze_size;i++){
		for(int j=0;j<maze_size;j++)
		{
		glBegin (GL_QUADS);
		glVertex3f (i*box_r-box_r/2,6,j*box_r-box_r/2);
		glVertex3f (i*box_r+box_r/2,6,j*box_r-box_r/2);
		glVertex3f (i*box_r+box_r/2,6,j*box_r+box_r/2);
		glVertex3f (i*box_r-box_r/2,6,j*box_r+box_r/2);
		glEnd ();


		}
		}
		glDisable(GL_BLEND);

		*/
		glColor4f(1, 1, 1, 1);
		glutSwapBuffers();
	}
}

void outPutMaze() {
	for (int i = 0; i < maze_size; i++) {
		for (int j = 0; j < maze_size; j++) {
			printf("%d ", maze[i][j]);
		}
		puts("");
	}
}

void testSeek() {
	outPutMaze();
	route_size = seek.seekroad(maze, maze_size, maze_size, route, 0, 1, 13, 13);
	outPutMaze();
	for (int i = 0; i < route_size; i++) {
		if (route[i] == 0)printf("→");
		if (route[i] == 1)printf("←");
		if (route[i] == 2)printf("↑");
		if (route[i] == 3)printf("↓");
		puts("");
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Maze");
	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	drawScore();

	createMyMenu();
	glutMouseFunc(myMouseFunc);
	glutSpecialFunc(inputKeyFunc);
	glutKeyboardFunc(processNormalKeys);

	loadModel();

	//testSeek(); //DONE!!

	glutMainLoop();
}