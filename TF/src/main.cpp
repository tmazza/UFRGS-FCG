#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <GL/glut.h>

#include "glm.h"

//openal (sound lib)
#include <AL/alut.h>

//bitmap class to load bitmaps for textures
#include "bitmap.h"

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
void initSound();
void initTexture();
void initModel();
void initLight();
void enableFog();
void createGLUI();
void mainRender();
void mainCreateMenu();
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

/**
Screen dimensions
*/
int windowWidth = 1000;
int windowHeight = 600;

/**
Screen position
*/
int windowXPos = 100;
int windowYPos = 150;

int mainWindowId = 0;

double xOffset = -1.9;
double yOffset = -1.3;
int mouseLastX = 0;
int mouseLastY = 0;

float roty = 0.0f;
float rotx = 90.0f;

bool rightPressed = false;
bool leftPressed = false;
bool upPressed = false;
bool downPressed = false;

bool spacePressed = false;

float speedX = 0.0f;
float speedY = 0.0f;
float speedZ = 0.0f;

float posX = 0.0f;
float posY = 0.4f;
float posZ = 2.0f;

/*
variavel auxiliar pra dar varia��o na altura do ponto de vista ao andar.
*/
float headPosAux = 0.0f;

float maxSpeed = 0.25f;

float planeSize = 8.0f;
int xQuads = 40;
int zQuads = 40;


// more sound stuff (position, speed and orientation of the listener)
ALfloat listenerPos[]={0.0,0.0,4.0};
ALfloat listenerVel[]={0.0,0.0,0.0};
ALfloat listenerOri[]={0.0,0.0,1.0,
						0.0,1.0,0.0};

// now the position and speed of the sound source
ALfloat source0Pos[]={ -2.0, 0.0, 0.0};
ALfloat source0Vel[]={ 0.0, 0.0, 0.0};

// buffers for openal stuff
ALuint  buffer[NUM_BUFFERS];
ALuint  source[NUM_SOURCES];
ALuint  environment[NUM_ENVIRONMENTS];
ALsizei size,freq;
ALenum  format;
ALvoid  *data;



// parte de c�digo extra�do de "texture.c" por Michael Sweet (OpenGL SuperBible)
// texture buffers and stuff
int i;                       /* Looping var */
BITMAPINFO	*info;           /* Bitmap information */
GLubyte	    *bits;           /* Bitmap RGB pixels */
GLubyte     *ptr;            /* Pointer into bit buffer */
GLubyte	    *rgba;           /* RGBA pixel buffer */
GLubyte	    *rgbaptr;        /* Pointer into RGBA buffer */
GLubyte     temp;            /* Swapping variable */
GLenum      type;            /* Texture type */
GLuint      texture;         /* Texture object */


bool crouched = false;
bool running = false;
bool jumping = false;
float jumpSpeed = 0.06;
float gravity = 0.004;
float posYOffset = 0.2;

float backgrundColor[4] = {0.0f,0.0f,0.0f,1.0f};

GLMmodel *modelSphere;
GLMmodel *modelOutro1;
GLMmodel *modelOutro2;
GLMmodel *modelCube;

struct entity {
	int x,y;
	float posx,posy;
};
typedef struct entity ENTITY;

ENTITY jogador;

void addObjetosNivel(int nivel){
	char filename[14] = "res/teste.bmp";
	float yBase = 0.2f;
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

	for(int i = height-1; i >= 0; i--) {
			fread(data, sizeof(unsigned char), row_padded, f);
			for(int j = 0; j < width*3; j += 3) {
					tmp = data[j]; data[j] = data[j+2];  data[j+2] = tmp;
					if(nivel == 1){
							glPushMatrix();
							glTranslatef(i*0.4-4.0,yBase,(j/3)*0.4-4.0);
							glmDraw(modelCube, GLM_SMOOTH  );
							glPopMatrix();
					} else {

					 if((int)data[j] == 255){ // Se R != 0
 					     glPushMatrix();
 					     glTranslatef(i*0.4-4.0,yBase,(j/3)*0.4-4.0);
 					     glmDraw(modelCube, GLM_SMOOTH );
 					     glPopMatrix();
					} else if((int)data[j+1] == 255) { // Se G != 0
					     glPushMatrix();
					     glTranslatef(i*0.4-4.0,yBase,(j/3)*0.4-4.0);
					     glmDraw(modelOutro1, GLM_SMOOTH | GLM_MATERIAL );
					     glPopMatrix();
					} else if((int)data[j+2] == 255) { // Se B != 0
					     glPushMatrix();
					     glTranslatef(i*0.4-4.0,yBase,(j/3)*0.4-4.0);
					     glmDraw(modelOutro2, GLM_SMOOTH | GLM_MATERIAL );
					     glPopMatrix();
					}
				}
			}
	}
	fclose(f);
}

void addObjetos() {
	addObjetosNivel(1);
	addObjetosNivel(2);

	glPushMatrix();
	glTranslatef(jogador.posx,0.6,jogador.posy);
	glmDraw(modelSphere, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE );
	glPopMatrix();

}


// Aux function to load the object using GLM and apply some functions
bool C3DObject_Load_New(const char *pszFilename, GLMmodel **model)
{
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
	gluLookAt(posX,posY + posYOffset + 0.025 * std::abs(sin(headPosAux*PI/180)),posZ,
		posX + sin(roty*PI/180),posY + posYOffset + 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180),posZ -cos(roty*PI/180),
		0.0,1.0,0.0);
}

/**
Atualiza a posi��o e orienta��o da camera
*/
void updateCam() {

	// Em 1º pessoal
	gluLookAt(posX,posY + posYOffset + 0.025 * std::abs(sin(headPosAux*PI/180)),posZ,
		posX + sin(roty*PI/180),posY + posYOffset + 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180),posZ -cos(roty*PI/180),
		0.0,1.0,0.0);

	// Camera aerea
	// gluLookAt(4.0,0.0,4.0,
	// 	posX + sin(roty*PI/180),posY + posYOffset + 0.025 * std::abs(sin(headPosAux*PI/180)) + cos(rotx*PI/180),posZ -cos(roty*PI/180),
	// 	0.0,1.0,0.0);

	// atualiza a posi��o do listener e da origen do som, s�o as mesmas da camera, j� que os passos vem de onde o personagem est�
	listenerPos[0] = posX;
	listenerPos[1] = posY;
	listenerPos[2] = posZ;
	source0Pos[0] = posX;
	source0Pos[1] = posY;
	source0Pos[2] = posZ;

  GLfloat light_position1[] = {posX, posY, posZ, 1.0 };
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

	initTexture();
	initModel();
	initLight();
  //enableFog();

	jogador.x=2;
	jogador.y=2;
	jogador.posx = jogador.x*0.4-4.0;
	jogador.posy = jogador.y*0.4-4.0;

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

/**
Initialize the texture using the library bitmap
*/
void initTexture(void)
{
  printf ("\nLoading texture..\n");
  // Load a texture object (256x256 true color)
  bits = LoadDIBitmap("res/tiledbronze.bmp", &info);
  if (bits == (GLubyte *)0) {
		printf ("Error loading texture!\n\n");
		return;
	}
  // Figure out the type of texture
  if (info->bmiHeader.biHeight == 1)
    type = GL_TEXTURE_1D;
  else
    type = GL_TEXTURE_2D;

  // Create and bind a texture object
  glGenTextures(1, &texture);
	glBindTexture(type, texture);

  // Create an RGBA image
  rgba = (GLubyte *)malloc(info->bmiHeader.biWidth * info->bmiHeader.biHeight * 4);

  i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
  for( rgbaptr = rgba, ptr = bits;  i > 0; i--, rgbaptr += 4, ptr += 3)
  {
          rgbaptr[0] = ptr[2];     // windows BMP = BGR
          rgbaptr[1] = ptr[1];
          rgbaptr[2] = ptr[0];
          rgbaptr[3] = (ptr[0] + ptr[1] + ptr[2]) / 3;
  }

	// Set texture parameters
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glTexImage2D(type, 0, 4, info->bmiHeader.biWidth, info->bmiHeader.biHeight,
                0, GL_RGBA, GL_UNSIGNED_BYTE, rgba );

  printf("Textura %d\n", texture);
	printf("Textures ok.\n\n");


}

void enableFog(void)
{
    float fog_colour[4] = {0,0,0,0};
    glEnable(GL_FOG);
    glFogf(GL_FOG_MODE,GL_LINEAR);
    glFogfv(GL_FOG_COLOR,fog_colour);
    glFogf(GL_FOG_DENSITY,0.1);
    glFogf(GL_FOG_START,0.1);
    glFogf(GL_FOG_END,5);
}

void setQuadAt(int i,int j){
	glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f);   // coords for the texture
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f(i * (float)planeSize/xQuads, 0.0f, (j+1) * (float)planeSize/zQuads);

			glTexCoord2f(0.0f, 0.0f);  // coords for the texture
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f((i+1) * (float)planeSize/xQuads, 0.0f, (j+1) * (float)planeSize/zQuads);

			glTexCoord2f(0.0f, 1.0f);  // coords for the texture
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f((i+1) * (float)planeSize/xQuads, 0.0f, j * (float)planeSize/zQuads);

			glTexCoord2f(1.0f, 1.0f);  // coords for the texture
			glNormal3f(0.0f,1.0f,0.0f);
			glVertex3f(i * (float)planeSize/xQuads, 0.0f, j * (float)planeSize/zQuads);
	glEnd();
}

void renderFloor() {
	// set things up to render the floor with the texture
	glShadeModel(GL_SMOOTH);
	glEnable(type);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPushMatrix();

  glTranslatef(-(float)planeSize/2.0f, 0.0f, -(float)planeSize/2.0f);

	float textureScaleX = 10.0;
	float textureScaleY = 10.0;
  glColor4f(1.0f,1.0f,1.0f,1.0f);

  for (int i = 0; i < xQuads; i++) {
      for (int j = 0; j < zQuads; j++) {
				setQuadAt(i,j);
      }
    }

	glDisable(type);


	glPopMatrix();
}

void renderScene() {
	glClearColor(backgrundColor[0],backgrundColor[1],backgrundColor[2],backgrundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpar o depth buffer

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	updateCam();
	glBindTexture(type, texture);
	addObjetos();
	renderFloor();
}

void updateState() {
	if (upPressed || downPressed) {
		if (running) {
			speedX = 0.05 * sin(roty*PI/180) * 2;
			speedZ = -0.05 * cos(roty*PI/180) * 2;
		} else {
			speedX = 0.05 * sin(roty*PI/180);
			speedZ = -0.05 * cos(roty*PI/180);
		}
		// efeito de "sobe e desce" ao andar
		headPosAux += 8.5f;
		if (headPosAux > 180.0f) {
			headPosAux = 0.0f;
		}
    if (upPressed) {

				jogador.posx += 0.01;

				posX += speedX;
        posZ += speedZ;
    } else {

			jogador.posy -= 0.01; 

				posX -= speedX;
        posZ -= speedZ;
    }
	} else {
		// parou de andar, para com o efeito de "sobe e desce"
		headPosAux = fmod(headPosAux, 90) - 1 * headPosAux / 90;
		headPosAux -= 4.0f;
		if (headPosAux < 0.0f) {
			headPosAux = 0.0f;
		}
	}

	posY += speedY;
	// if (posY < heightLimit) {
		// posY = heightLimit;
		// speedY = 0.0f;
		// jumping = false;
	// } else {
		// speedY -= gravity;
	// }

	if (crouched) {
		posYOffset -= 0.01;
		if (posYOffset < 0.1) {
			posYOffset = 0.1;
		}
	} else {
		posYOffset += 0.01;
		if (posYOffset > 0.2) {
			posYOffset = 0.2;
		}
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
	usleep(1000);
}

/**
Handles events from the mouse right button menu
*/
void mainHandleMouseRightButtonMenuEvent(int option) {
	switch (option) {
		case 1 :
			exit(0);
			break;
		default:
			break;
	}
}

/**
Create mouse button menu
*/
void mainCreateMenu() {
	glutCreateMenu(mainHandleMouseRightButtonMenuEvent);
	glutAddMenuEntry("Quit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/**
Mouse button event handler
*/
void onMouseButton(int button, int state, int x, int y) {
	//printf("onMouseButton button: %d \n", button);
	glutPostRedisplay();
}

/**
Mouse move while button pressed event handler
*/
void onMouseMove(int x, int y) {

	/*mouseLastX = x;
	mouseLastY = y;*/

	glutPostRedisplay();
}

/**
Mouse move with no button pressed event handler
*/
void onMousePassiveMove(int x, int y) {

	roty += (x - mouseLastX);
	rotx -= (y - mouseLastY);

	if (rotx < -128.0) {
		rotx = -128.0;
	}

	if (rotx > -45.0) {
		rotx = -45.0;
	}

	mouseLastX = x;
	mouseLastY = y;

	//glutPostRedisplay();
}

/**
Key press event handler
*/
void onKeyDown(unsigned char key, int x, int y) {
	//printf("%d \n", key);
	switch (key) {
		case 32: //space
			if (!spacePressed && !jumping) {
				jumping = true;
				speedY = jumpSpeed;
			}
			spacePressed = true;
		break;
		case 'w': upPressed = true; break;
		case 's': downPressed = true; break;
		case 'a': leftPressed = true; break;
		case 'd': rightPressed = true; break;
		case 'c': crouched = true; break;
		case 'r': running = true; break;
		default:
			break;
	}
	//glutPostRedisplay();
}

/**
Key release event handler
*/
void onKeyUp(unsigned char key, int x, int y) {
	switch (key) {
		case 32: //space
			spacePressed = false;
			break;
		case 119: //w
			upPressed = false;
			break;
		case 115: //s
			downPressed = false;
			break;
		case 97: //a
			leftPressed = false;
			break;
		case 100: //d
			rightPressed = false;
			break;
		case 99: //c
			crouched = false;
			break;
		case 114: //r
			running = false;
			break;
		case 27:
			exit(0);
			break;
		default:
			break;
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
