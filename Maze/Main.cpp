#include "stdafx.h"
#include "tag.h"
#include "Testmd2.h"
#include <GL/glut.h>
#include <GL/glaux.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

//�Լ���д��
#include "seekRoad.h"

#define WALL 1
#define GROUND 2
#define MAZE_WALK 1
#define MAZE_EDIT 2
#define FIRST_PERSON 1
#define TRIRD_PERSON 3
//ȫ�ֱ���

GLuint texture[2];//�洢��������
float wall[8][3];//��Ż�ǽ��������

int window_width = 630;
int window_height = 630;
int crt_window = MAZE_WALK; //1Ϊ�Թ����ڣ�2Ϊ�����Թ�����
int draw_which = 0; //WALL =1 GROUND =2
int maze_size = 20;
int view = TRIRD_PERSON; //Ĭ�ϵ����˳�
double crt_pos_x, crt_pos_z, save_pos_x, save_pos_z, look_x, look_z, eye_x, eye_z;
double angle, distant = 6, angleM = 90;//�ӽǣ���Ұ,ģ��
double bili = 4;//�����ӽ��¼�������Ļ�������
double box_r = 7;//ǽ���С
SeekRoad seek;

int maze[105][105]= {
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,1,1,0,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,0,0,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }
};
int new_maze[105][105];//maze:��ǰ�Թ� new_maze:��ǰ�༭���Թ�
int route[10000],route_size;

void setLookEye() {
	//���ӽ��뵱ǰλ�ö�Ӧ
	look_x = crt_pos_x + distant*cos(angle);
	look_z = crt_pos_z - distant*sin(angle);
	eye_x = crt_pos_x - distant*cos(angle);
	eye_z = crt_pos_z + distant*sin(angle);
}

void display();
void switchWindow(int window);

void moveTo(int x, int z) {
	int crt_r = (int)((crt_pos_z + box_r / 2) / box_r);
	int crt_c = (int)((crt_pos_x + box_r / 2) / box_r);
	printf("crt: r:%d c:%d Moveto r:%d c:%d\n",crt_r ,crt_c ,z, x);
	route_size = seek.seekroad(maze, maze_size, maze_size, route, crt_c, crt_r, x, z);
	if (route_size == -1) {
		printf("���ɵ���\n");
		return;
	}
	for (int i = 0; i < route_size; i++) {
		for (int j = 0; j < box_r; j++) {
			crt_pos_z += seek.mv[route[i]][0];
			crt_pos_x += seek.mv[route[i]][1]; 
			setLookEye();
			display();
		}
	}

}

void menu(int id) {
	switch (id) {
	case 1:
		view = FIRST_PERSON;
		glutPostRedisplay();
		break;
	case 2:
		view = TRIRD_PERSON;
		glutPostRedisplay();
		break;
	case 3:
		save_pos_x = crt_pos_x;
		save_pos_z = crt_pos_z;
		glutPostRedisplay();
		break;
	case 4:
		if (view == TRIRD_PERSON)
			moveTo(save_pos_x, save_pos_z);
		else {
			crt_pos_z = save_pos_z;
			crt_pos_x = save_pos_x;
		}
		setLookEye();
		glutPostRedisplay();
		break;

	case 5:
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
	case 3://����
		memcpy(maze, new_maze, sizeof maze);
		break;
	case 4://�˳�
		switchWindow(MAZE_WALK);
		break;
	case 5://���
		memset(new_maze, 0, sizeof new_maze);
		for (int i = 0; i<20; i++)
			new_maze[0][i] = new_maze[maze_size -1][i] = new_maze[i][0] = new_maze[i][maze_size -1] = 1;
		new_maze[1][0] = 0;//�����
		glutPostRedisplay();
		break;
	}
}

void createMyMenu() {
	if (crt_window == 1) {
		glutCreateMenu(menu);
		glutAddMenuEntry("��һ�ӽ�", 1);
		glutAddMenuEntry("�����ӽ�", 2);
		glutAddMenuEntry("���浱ǰλ��", 3);
		glutAddMenuEntry("������һλ��", 4);
		glutAddMenuEntry("�����Թ�", 5);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
	}
	else if(crt_window == 2){
		glutCreateMenu(newMenu);
		glutAddMenuEntry("����ǽ", 1);
		glutAddMenuEntry("���Ʋݵ�", 2);
		glutAddMenuEntry("����", 3);
		glutAddMenuEntry("�����Թ�����", 4);
		glutAddMenuEntry("���", 5);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
	}
}

void switchWindow(int window) {
	crt_window = window;
	createMyMenu();
}

void myMouseFunc(int button,int state,int x,int y){
	x = x / bili - 10;
	y = y / bili - 10;//����Ļ�������¼����Ӧ

	int new_maze_x = (int)((x + box_r/2) / box_r);//����Ļ�����ӦΪ�����±�
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

		//���浱ǰλ�õ�λ���Թ��У������»���С��ͼ
		//savePosMaze();

		//��ײ���

		for (int i = 0; i<maze_size; i++){
			for (int j = 0; j<maze_size; j++){
				if (maze[j][i] == 1){
					float maze_pos_x = (float)i*box_r;
					float maze_pos_z = (float)j*box_r;
					//cout<< "maze_pos_x  "<<maze_pos_x<<"    maze_pos_z  "<<maze_pos_z<<endl;
					int model_w = 3;
					if (view == 1)model_w = 1;
					else model_w = 3;
					//�ж�ǽ�ĺ���Լ�����Ŀ���뵱ǰλ��
					if (crt_pos_x>(maze_pos_x - box_r / 2 - model_w) && crt_pos_x<(maze_pos_x + box_r / 2 + model_w) && 
						crt_pos_z>(maze_pos_z - box_r / 2 - model_w) && crt_pos_z<(maze_pos_z + box_r / 2 + model_w))
					{
						crt_pos_x = oldpos_x;
						crt_pos_z = oldpos_z;
						//std::cout << "ǽ�� " << " maze_pos_x  " << j << "  maze_pos_z  " << i << std::endl;
					}
				}
			}
		}
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
	default:
		break;
	}

}

AUX_RGBImageRec *LoadBMP(char *Filename) {		// ����λͼͼ�� 		
	FILE *File = NULL;

	if (!Filename) {								//ȷ���������ļ�����Ч
		return NULL;							// ���û�ṩ������ NULL   
	}

	File = fopen(Filename, "r");					// ���ļ�	

	if (File) {
		fclose(File);							// �ر��ļ����	
		File = NULL;
		return auxDIBImageLoad(Filename);       // ����λͼ������ָ��
	}

	return NULL;								// �������ʧ�ܣ����� NULL   
}

int loadGLTextures() {        // ���ݼ��ص�λͼ��������
	int Status = FALSE;         // ָʾ�������Ƿ�ɹ��ı�־

	AUX_RGBImageRec *TextureImage[2];     // ����һ������ͼ�����飬����ָ�������СΪ6

	memset(TextureImage, 0, sizeof(void *) * 2);           // ��ʼ������ͼ�����飬Ϊ������ڴ�

	char *pictures[] = { // ����һ��λͼ�������飬��Ӧ2��λͼ
		"wall.bmp",
		"ground.bmp",

	};
	for (int i = 0; i<2; i++) { // ����λͼ�������飬����λͼ���Ʒֱ�����
		if (TextureImage[i] = LoadBMP(pictures[i])) { // ����λͼi�ɹ����޸�״̬��־����StatusΪTRUE
			Status = TRUE;

			glGenTextures(1, &texture[i]);     // Ϊ��i��λͼ��������
			glBindTexture(GL_TEXTURE_2D, texture[i]); // �����ɵ���������ư󶨵�ָ����������
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[i]->sizeX, TextureImage[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[i]->data);//����ָ����ά����ͼ��
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//ʵ�������˲��Ĺ��ܣ�������ӳ�䵽ͼ�α����Ժ������Ϊ�������������õ��������ܸ��õ���ʾ��ʱ�򣬽��й��ˣ�����ָ���ķ�ʽ������ʾ�����ܻ���˵���ʾ���������������ء�
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//�����˲�
		}

		if (TextureImage[i]) {         // �����Ƿ����
			if (TextureImage[i]->data) { // ����ͼ���Ƿ����      
				free(TextureImage[i]->data); // �ͷ�����ͼ��ռ�õ��ڴ�  
			}
			free(TextureImage[i]); // �ͷ�ͼ��ṹ       
		}
	}
	return Status;          // �����������أ����سɹ�����ʧ�ܵı�־Status
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
		glBindTexture(GL_TEXTURE_2D, texture[0]); // ѡ���һ������texture[0]������������
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
	//glClearDepth(1.0f);//������Ȼ���
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// �����Ļ����Ȼ���
													   //glDepthFunc(GL_LEQUAL);//���С����ȵ�ʱ��Ҳ��Ⱦ
													   //glShadeModel(GL_SMOOTH);// ������Ӱƽ��
	crt_pos_x = 0; crt_pos_z = box_r;
	setLookEye();
}

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

void loadModel() {
	float minmax[6];

	// load 3D information of the model tris.md2
	md2_model_2 = md2_readModel("WalkMech.md2");
	md2_getBoundingBox(md2_model_2, minmax);

	model_center_2[0] = (minmax[0] + minmax[1]) / 2.0f;
	model_center_2[1] = (minmax[2] + minmax[3]) / 2.0f;
	model_center_2[2] = (minmax[4] + minmax[5]) / 2.0f;
	model_radius_2 = sqrt((minmax[0] - minmax[1]) * (minmax[0] - minmax[1]) +
		(minmax[2] - minmax[3]) * (minmax[2] - minmax[3]) +
		(minmax[4] - minmax[5]) * (minmax[4] - minmax[5])) / 2.0;

	loadTexture("GenericMech.tga", Decal_Texture);
}

void drawGround() {

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[1]); // ѡ��ڶ�������texture[1]������������
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);  glVertex3f(-box_r / 2, -box_r / 2, -box_r / 2);
	glTexCoord2f(1.0f, 1.0f);  glVertex3f(-box_r / 2, -box_r / 2, box_r*maze_size - box_r / 2);
	glTexCoord2f(1.0f, 0.0f);  glVertex3f(box_r*maze_size - box_r / 2, -box_r / 2, box_r*maze_size - box_r / 2);
	glTexCoord2f(0.0f, 0.0f);  glVertex3f(box_r*maze_size - box_r / 2, -box_r / 2, -box_r / 2);
	glEnd();
	glPopMatrix();

}

void display() {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor4f(1, 1, 1, 1);
	
	if (crt_window == MAZE_WALK) {
		//����ģ��


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, Decal_Texture);

		glPushMatrix();
		glTranslatef(crt_pos_x, -1.5, crt_pos_z);//ģ��λ��
		glScalef(0.05*view, 0.05*view, 0.05*view);//����ģ�ʹ�С����������ӽ�ʱ��ģ��̫С�����ù۲�

		glRotatef(angleM, 0.0, 1.0, 0.0);//ģ�ͷ���

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


			//������
			drawGround();
			
			//���Թ�
			for (int i = 0; i<maze_size; i++)
			{
				for (int j = 0; j<maze_size; j++)
				{
					if (maze[j][i] == 1)
					{
						glPushMatrix();
						glTranslatef((float)i*box_r, 0.0f, (float)j*box_r);
						makeBox(box_r, box_r, box_r);
						glPopMatrix();
					}
				}
			}
			
			glutSwapBuffers();


		}
		
		
		if (view == FIRST_PERSON) {

			//С��ͼ

			glViewport(window_width - 200, window_height - 200, 200, 200);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glOrtho(-10, 150, -150, 10, -10, 10);
			glRotatef(90, 1.0f, 0.0f, 0.0f);

			//������
			drawGround();

			//���Թ�
			for (int i = 0; i<maze_size; i++)
			{
				for (int j = 0; j<maze_size; j++)
				{
					if (maze[j][i] == 1)
					{
						glPushMatrix();
						glTranslatef((float)i*box_r, 0.0f, (float)j*box_r);
						makeBox(box_r, box_r, box_r);
						glPopMatrix();
					}
				}
			}
			//����ģ��


			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glEnable(GL_DEPTH_TEST);

			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, Decal_Texture);


			//Tris
			glPushMatrix();
			glTranslatef(crt_pos_x, 3, crt_pos_z);//ģ��λ��
			glScalef(0.2, 0.2, 0.2);//����ģ�ʹ�С

			glRotatef(angleM, 0.0, 1.0, 0.0);//ģ�ͷ���

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




			//��һ�ӽǵĴ�ͼ
			// �л���ͶӰģʽ

			glViewport(0, 0, window_width, window_height);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			gluPerspective(30.0, (double)window_width / (double)window_height, 1.0f, 100.0f);
			gluLookAt(eye_x, 0.5f, eye_z, // �ӵ㣬Ϊ���ܿ���ģ�ͣ������ӵ�������ģ��Ϊ���ĵ�Բ�ϡ�
				look_x, 0.0f, look_z, 0.0f, 1.0f, 0.0f);

			glMatrixMode(GL_MODELVIEW);

			glLoadIdentity();

			//������
			drawGround();
			//���Թ�
			for (int i = 0; i<maze_size; i++)
			{
				for (int j = 0; j<maze_size; j++)
				{
					if (maze[j][i] == 1)
					{
						glPushMatrix();
						glTranslatef((float)i*box_r, 0.0f, (float)j*box_r);
						makeBox(box_r, box_r, box_r);
						glPopMatrix();
					}
				}
			}
			glutSwapBuffers();

		}
		
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
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
		if (route[i] == 0)printf("��");
		if (route[i] == 1)printf("��");
		if (route[i] == 2)printf("��");
		if (route[i] == 3)printf("��");
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
	createMyMenu();
	glutMouseFunc(myMouseFunc);
	glutSpecialFunc(inputKeyFunc);
	glutKeyboardFunc(processNormalKeys);

	loadModel();

	//testSeek(); //DONE!!

	glutMainLoop();
}