#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <GL/glut.h>
#include "glm.h"
#include "bitmap.h" //bitmap class to load bitmaps for textures
#include "Camera.h"
#include "Bloco.h"

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "alut.lib")

#define PI 3.14159265

// sound stuff
#define NUM_BUFFERS 1
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 1

#define SMOOTH 0
#define SMOOTH_MATERIAL 1
#define SMOOTH_MATERIAL_TEXTURE 2

using namespace std;
void mainInit();
void initModel();
void initLight();
void createGLUI();
void mainRender();
void onMouseButton(int button, int state, int x, int y);
void onMouseMove(int x, int y);
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onGLUIEvent(int id);
void onWindowReshape(int x, int y);
void mainIdle();
int main(int argc, char **argv);
void setWindow();
void setViewport(GLint left, GLint right, GLint bottom, GLint top);
void updateState();
void renderFloor();
void updateCam();

int windowWidth = 1000;
int windowHeight = 600;
int windowXPos = 100;
int windowYPos = 150;
int mainWindowId = 0;

int mouseLastX = 0;
int mouseLastY = 0;

bool emTeste = false;


float planeSize = 100.0f;
int xQuads = 40;
int zQuads = 40;
float backgrundColor[4] = {0.0f,0.0f,0.0f,1.0f};

GLMmodel *modelSphere;
GLMmodel *modelOutro1;
GLMmodel *modelOutro2;
GLMmodel *modelCube;

Camera cam;
Bloco nivel1[20][20];
Bloco nivel2[20][20];
Bloco *jog;


void addObjetosNivel(int nivel){
	char filename[14] = "res/teste.bmp";
	float yBase = 0.4f;
	if(nivel == 2){
		filename[8] = 'i';
		yBase += 0.4f;
	}
	int i;
	FILE* f = fopen(filename, "rb");

	if(f == NULL){
		throw "---------->Nao achei o mapa! :(";
	}

	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];
	int row_padded = (width*3 + 3) & (~3);
	unsigned char* data = new unsigned char[row_padded];
	unsigned char tmp;
	int idx;
	bool cR,cG,cB;
	for(int i = height-1; i >= 0; i--) {
			fread(data, sizeof(unsigned char), row_padded, f);
			for(int j = 0; j < width*3; j += 3) {
					idx = j/3;
					tmp = data[j]; data[j] = data[j+2]; data[j+2] = tmp;
					cR = (int)data[j] == 255;
					cG = (int)data[j+1] == 255;
					cB = (int)data[j+2] == 255;
					if(nivel == 1){
							if(!cR &&  cG && !cB) nivel1[i][idx].instanciar('P',i, j/3, yBase,(GLMmodel *) modelCube);
							else nivel1[i][idx].instanciar('V',i, j/3, yBase,(GLMmodel *) modelCube);
							nivel1[i][idx].render();
					} else {

					/** J: 001 | jogador
						* I: 100 | inimigo
						* P: 010 | parede (ou bloco conforme especificação)
						* B: 110 | buraco
						* R: 101 | rachadura
						* V: ??? | vazio */
						if(!cR && !cG && cB)
						{
							 nivel2[i][idx].instanciar('J',i, j/3, yBase,(GLMmodel *) modelCube);
							 jog = &nivel2[i][idx];
						}
						else if( cR && !cG && !cB) nivel2[i][idx].instanciar('I',i, j/3, yBase,(GLMmodel *) modelCube);
						else if(!cR &&  cG && !cB) nivel2[i][idx].instanciar('P',i, j/3, yBase,(GLMmodel *) modelCube);
						else if( cR &&  cG && !cB) nivel2[i][idx].instanciar('B',i, j/3, yBase,(GLMmodel *) modelCube);
						else if( cR && !cG &&  cB) nivel2[i][idx].instanciar('R',i, j/3, yBase,(GLMmodel *) modelCube);
						else nivel2[i][idx].instanciar('V',i, j/3, yBase,(GLMmodel *) modelCube);

						nivel2[i][idx].render();

					}
			}
	}
	fclose(f);
}

void addObjetos() {
	addObjetosNivel(1);
	addObjetosNivel(2);

	// a.modelo = (GLMmodel *) modelSphere;
	// a.render();
	// glPushMatrix();
	// glTranslatef(jogador.posx,0.6,jogador.posy);
	// glmDraw(modelSphere, GLM_SMOOTH );
	// glPopMatrix();

}

void renderFloor() {
	glPushMatrix();
    glTranslatef(-(float)planeSize/2.0f, 0.0f, -(float)planeSize/2.0f);

		int xQuads = 40;
    int zQuads = 40;
    for (int i = 0; i < xQuads; i++) {
        for (int j = 0; j < zQuads; j++) {
            glBegin(GL_QUADS);
                glVertex3f(i * (float)planeSize/xQuads, 0.0f, (j+1) * (float)planeSize/zQuads);
                glVertex3f((i+1) * (float)planeSize/xQuads, 0.0f, (j+1) * (float)planeSize/zQuads);
                glVertex3f((i+1) * (float)planeSize/xQuads, 0.0f, j * (float)planeSize/zQuads);
								glVertex3f(i * (float)planeSize/xQuads, 0.0f, j * (float)planeSize/zQuads);
            glEnd();
        }
    }
	glPopMatrix();
}

// Aux function to load the object using GLM and apply some functions
bool C3DObject_Load_New(const char *pszFilename, GLMmodel **model) {
		char aszFilename[256];
    strcpy(aszFilename, pszFilename);

    if (*model) {

    free(*model);
    *model = NULL;
    }

    *model = glmReadOBJ(aszFilename);
    if (!(*model))
    return false;

	//		GLfloat sFactor[] = { 0.5, 0.5, 0.5, 1.0 };
    glmUnitize(*model);
  	glmScale(*model,0.195); // USED TO SCALE THE OBJECT
    glmFacetNormals(*model);
    glmVertexNormals(*model, 90.0);

    return true;
}

void setWindow() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cam.x,cam.y,cam.z,
		cam.x + sin(cam.roty*PI/180),cam.y + cos(cam.rotx*PI/180),cam.z -cos(cam.roty*PI/180),
		0.0,1.0,0.0);
}

/**
Atualiza a posi��o e orienta��o da camera
*/
void updateCam() {

	// Em 1º pessoal
	gluLookAt(cam.x,cam.y,cam.z,
		cam.x + sin(cam.roty*PI/180),cam.y + cos(cam.rotx*PI/180),cam.z -cos(cam.roty*PI/180),
		0.0,1.0,0.0);

	// Camera aerea
	// gluLookAt(0.0,6.0,6.0,
	// 	cam.x + sin(cam.roty*PI/180),cam.y + cos(cam.rotx*PI/180),cam.z -cos(cam.roty*PI/180),
	// 	0.0,1.0,0.0);

  GLfloat light_position1[] = {cam.x, cam.y, cam.z, 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light_position1);


}

void initLight() {
  glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	GLfloat light_ambient[] = { backgrundColor[0], backgrundColor[1], backgrundColor[2], backgrundColor[3] };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position1[] = {0.0, 0.0, 0.0, 1.0 };
  GLfloat spotlight_position1[] = {0.0, 0.0, 0.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, spotlight_position1);

}

void setViewport(GLint left, GLint right, GLint bottom, GLint top) {
	glViewport(left, bottom, right - left, top - bottom);
}

/**
Initialize
*/
void mainInit() {
	glClearColor(1.0,1.0,1.0,0.0);
	glColor3f(0.0f,0.0f,0.0f);
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);

	// habilita o z-buffer
	glEnable(GL_DEPTH_TEST);

	initModel();
	initLight();

	printf("w - andar \n");
	printf("s - ir pra tras \n");
	printf("mouse - direcao \n");
	printf("r - correr \n");
	printf("c - abaixar \n");
	printf("espaco - pular \n");

}

void initModel() {
	printf("Loading models.. \n");
	C3DObject_Load_New("res/ball.obj",&modelSphere);
	C3DObject_Load_New("res/porsche.obj",&modelOutro1);
	C3DObject_Load_New("res/dolphins.obj",&modelOutro2);
	C3DObject_Load_New("res/cube.obj",&modelCube);
	printf("Models ok. \n \n \n");
}

void renderScene() {
 	glClearColor(backgrundColor[0],backgrundColor[1],backgrundColor[2],backgrundColor[3]);
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpar o depth buffer
 	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();
 	updateCam();
 	addObjetos();
 	renderFloor();
}

void updateState() {
	cam.update();

	int i,xn1,yn1;
	for(i=0;i<400;i++){
		nivel1[i/20][i%20].update();
		nivel2[i/20][i%20].update();
		// Verifica se elemento de nivel 2 está em cima de um elemento válido do nível 1
		if(nivel2[i/20][i%20].tipo == 'J'){
			xn1 = nivel2[i/20][i%20].x;
			yn1 = nivel2[i/20][i%20].y;
			if(!nivel1[xn1][yn1].ativo){
				nivel2[i/20][i%20].emQueda = true;
			}
		}
	}

	if(emTeste){
		for(i=0;i<50;i++){
			nivel1[i/20][i%20].emQueda = true;
			// nivel2[i/20][i%20].emQueda = true;
		}
		// printf("JOGO: %d, %d\n",jog->x,jog->y);
		// printf("\n\n");
	}

}

/**
Render scene
*/
void mainRender() {
	updateState();
	renderScene();
	glFlush();
	glutPostRedisplay();
	usleep(100);
}

/**
Mouse button event handler
*/
void onMouseButton(int button, int state, int x, int y) {
	// printf("onMouseButton button: %d | %d | %d \n", button,x,y);
	glutPostRedisplay();
}

/**
Mouse move while button pressed event handler
*/
void onMouseMove(int x, int y) {
	glutPostRedisplay();
}

/**
Mouse move with no button pressed event handler
*/
void onMousePassiveMove(int x, int y) {
	cam.roty += (x - mouseLastX);
	cam.rotx -= (y - mouseLastY);
	if (cam.rotx < -128.0)
		cam.rotx = -128.0;
	if (cam.rotx > -45.0)
		cam.rotx = -45.0;
	mouseLastX = x;
	mouseLastY = y;
}

/**
Key press event handler
*/
void onKeyDown(unsigned char key, int x, int y) {
	switch (key) {
		// case 32: spacePressed = true; break;
		case 'o': cam.frentePressed = true; break;
		case 'l': cam.trasPressed = true; break;
		case 'i': cam.upPressed = true; break;
		case 'p': cam.downtPressed = true; break;
		case 'w': jog->andaPressed = true; break;
		case 's': jog->voltaPressed = true; break;
		case 'a': jog->giraEsqPressed = true; break;
		case 'd': jog->giraDirPressed = true; break;

		case 't': emTeste = true; break;

		default: break;
	}
	//glutPostRedisplay();
}

/**
Key release event handler
*/
void onKeyUp(unsigned char key, int x, int y) {
	switch (key) {
		// case 32: spacePressed = false; break;
		case 'o': cam.frentePressed = false;	break;
		case 'l': cam.trasPressed = false; break;
		case 'i': cam.upPressed = false; break;
		case 'p': cam.downtPressed = false; break;
		case 'w': jog->andaPressed = false; break;
		case 's': jog->voltaPressed = false; break;
		case 'a': jog->giraEsqPressed = false; break;
		case 'd': jog->giraDirPressed = false; break;
		case 27: exit(0); break;

		case 't': emTeste = false; break;

		default: break;
	}
	//glutPostRedisplay();
}

void onWindowReshape(int x, int y) {
	windowWidth = x;
	windowHeight = y;
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);
}

/**
Glut idle funtion
*/
void mainIdle() {
	/**
	Set the active window before send an glutPostRedisplay call
	so it wont be accidently sent to the glui window
	*/
	glutSetWindow(mainWindowId);
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(windowWidth,windowHeight);
	glutInitWindowPosition(windowXPos,windowYPos);

	/**
	Store main window id so that glui can send it redisplay events
	*/
	mainWindowId = glutCreateWindow("FPS");

	glutDisplayFunc(mainRender);

	glutReshapeFunc(onWindowReshape);

	/**
	Register mouse events handlers
	*/
	glutMouseFunc(onMouseButton);
	glutMotionFunc(onMouseMove);
	glutPassiveMotionFunc(onMousePassiveMove);

	/**
	Register keyboard events handlers
	*/
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);

	mainInit();

	glutMainLoop();

    return 0;
}
