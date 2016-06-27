#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <map>
#include <iostream>
#include <GL/glut.h>
#include "glm.h"
#include "bitmap.h" //bitmap class to load bitmaps for textures
#include "Camera.h"
#include "Bloco.h"
#include "Partida.h"

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "alut.lib")

#define PI 3.14159265

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
void criaRachadura(int x,int y,int direcao);

int windowWidth = 1000;
int windowHeight = 600;
int windowXPos = 100;
int windowYPos = 150;
int mainWindowId = 0;
int subWindowId = 0;

int mouseLastX = 0;
int mouseLastY = 0;

bool emTeste = false;
bool spacePressed = false;

float planeSize = 50.0f;
int xQuads = 40;
int zQuads = 40;
float backgrundColor[4] = {0.0f,0.0f,0.0f,1.0f};

GLMmodel *modelSphere;
GLMmodel *modelCube;

GLMmodel *modelChao;
GLMmodel *modelRach;
GLMmodel *modelBurr;
GLMmodel *modelRock;
GLMmodel *modelPlayer;

int count;
Camera cam;
Bloco nivel1[20][20];
Bloco nivel2[20][20];
Bloco jog;
Partida partida;

int qtdIni;
map< int,Bloco> inimigos;
map< int,Bloco>::iterator it;

void addObjetosNivel(int nivel){
	char filename[14] = "res/teste.bmp";
	float yBase = 0.8f;
	if(nivel == 2){
		filename[8] = 'i';
		yBase += 0.4f;
	} else if(nivel == 3) {
		filename[8] = 'j';
		yBase += 1.0f;
	} else if(nivel == 4) {
		filename[8] = 'k';
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
	count = 0;
	qtdIni = 0;
	for(int i = height-1; i >= 0; i--) {
			fread(data, sizeof(unsigned char), row_padded, f);
			for(int j = 0; j < width*3; j += 3) {
					idx = j/3;
					tmp = data[j]; data[j] = data[j+2]; data[j+2] = tmp;
					cR = (int)data[j] == 255;
					cG = (int)data[j+1] == 255;
					cB = (int)data[j+2] == 255;
					if(nivel == 1){
							if(!cR &&  cG && !cB) nivel1[i][idx].instanciar('P',i, j/3, yBase,(GLMmodel *) modelChao);
							else nivel1[i][idx].instanciar('V',i, j/3, yBase,(GLMmodel *) modelCube);
							nivel1[i][idx].render();
					} else if(nivel == 2) {
						if(!cR && !cG && cB){ // J: 001 | jogador
							 nivel2[i][idx].instanciar('V',i, j/3, yBase,(GLMmodel *) modelCube);
							 jog.instanciar('J',i, j/3, yBase,(GLMmodel *) modelPlayer);
						}
						else if( cR && !cG && !cB){ // I: 100 | inimigo
							nivel2[i][idx].instanciar('V',i, j/3, yBase,(GLMmodel *) modelCube);
							inimigos[qtdIni].instanciar('I',i, j/3, yBase,(GLMmodel *) modelSphere);
							qtdIni++;
						}
						else if(!cR &&  cG && !cB) nivel2[i][idx].instanciar('P',i, j/3, yBase,(GLMmodel *) modelRock); // P: 010 | parede
						else if( cR &&  cG && !cB) nivel2[i][idx].instanciar('B',i, j/3, yBase,(GLMmodel *) modelBurr); // B: 110 | buraco
						else if( cR && !cG &&  cB) nivel2[i][idx].instanciar('R',i, j/3, yBase,(GLMmodel *) modelRach); // R: 101 | rachadura
						else nivel2[i][idx].instanciar('V',i, j/3, yBase,(GLMmodel *) modelSphere); // V: * | vazio
						nivel2[i][idx].id = count;
						nivel2[i][idx].render();
					} else if(nivel == 3 || nivel == 4){
						if(!cR &&  !cG && !cB) nivel1[i][idx].metamorfisa('V',(GLMmodel *) modelCube);
						else nivel1[i][idx].metamorfisa('P',(GLMmodel *) modelCube);
						nivel1[i][idx].posy = yBase;
						nivel1[i][idx].render();
					}
					count++;
			}
	}
	fclose(f);
}

void addObjetos() {
	addObjetosNivel(1);
	addObjetosNivel(2);
	jog.render();
	for (it = inimigos.begin() ; it != inimigos.end(); ++it)
		it->second.render();

	int x,y,emQueda;
	for(int i=0;i<400;i++){
		x = i/20; y = i%20;
		if(nivel2[x][y].tipo == 'B' || nivel2[x][y].tipo == 'R'){
			emQueda = true;
			if(x > 0 ) {
				if(nivel1[x-1][y].ativo && nivel2[x-1][y].tipo != 'R' && nivel2[x-1][y].tipo != 'B') { emQueda = false; }
				if(y > 0)
					if(nivel1[x-1][y-1].ativo && nivel2[x-1][y-1].tipo != 'R' && nivel2[x-1][y-1].tipo != 'B') { emQueda = false; }
				if(y < 19)
					if(nivel1[x-1][y+1].ativo && nivel2[x-1][y+1].tipo != 'R' && nivel2[x-1][y+1].tipo != 'B') { emQueda = false; }
			}
			if(x < 19) {
				if(  nivel1[x+1][y].ativo   && nivel2[x+1][y].tipo != 'R' && nivel2[x+1][y].tipo != 'B') { emQueda = false; }
				if(y > 0)
					if(nivel1[x+1][y-1].ativo && nivel2[x+1][y-1].tipo != 'R' && nivel2[x+1][y-1].tipo != 'B') { emQueda = false; }
				if(y < 19)
					if(nivel1[x+1][y+1].ativo && nivel2[x+1][y+1].tipo != 'R' && nivel2[x+1][y+1].tipo != 'B') { emQueda = false; }
			}
			if(y > 0)
				if(nivel1[x][y-1].ativo && nivel2[x][y-1].tipo != 'R' && nivel2[x][y-1].tipo != 'B') { emQueda = false; }
			if(y < 19)
				if(nivel1[x][y+1].ativo && nivel2[x][y+1].tipo != 'R' && nivel2[x][y+1].tipo != 'B') { emQueda = false; }

			if(emQueda){
				nivel2[x][y].emQueda = true;
				nivel1[x][y].emQueda = true;
			}
		}
	}
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
bool C3DObject_Load_New(const char *pszFilename, GLMmodel **model,float escala) {
		char aszFilename[256];
    strcpy(aszFilename, pszFilename);

    if (*model) {

    free(*model);
    *model = NULL;
    }
    *model = glmReadOBJ(aszFilename);
    if (!(*model))
    return false;
    glmUnitize(*model);
  	glmScale(*model,escala); // USED TO SCALE THE OBJECT
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
}

void initLight() {
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	GLfloat light_ambient[] = { backgrundColor[0], backgrundColor[1], backgrundColor[2], backgrundColor[3] };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position1[] = {0.0, 1.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);

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

void subInit(){
	glClearColor(1.0,1.0,1.0,0.0);
	glColor3f(0.0f,0.0f,0.0f);
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);

	// habilita o z-buffer
	glEnable(GL_DEPTH_TEST);

	initModel();
	initLight();
}


void initModel() {
	printf("Loading models.. \n");
	C3DObject_Load_New("res/sphe.obj",&modelSphere,0.2);
	C3DObject_Load_New("res/cube.obj",&modelCube,0.2);
	C3DObject_Load_New("res/chao.obj",&modelChao,0.2);
	C3DObject_Load_New("res/rach.obj",&modelRach,0.1999);
	C3DObject_Load_New("res/burr.obj",&modelBurr,0.1999);
	C3DObject_Load_New("res/rock.obj",&modelRock,0.2);
	C3DObject_Load_New("res/Minion.obj",&modelPlayer,0.3);

	printf("Models ok. \n \n \n");
}

void renderScene() {
 	glClearColor(backgrundColor[0],backgrundColor[1],backgrundColor[2],backgrundColor[3]);
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpar o depth buffer
 	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();
	cam.updatePosicao(jog);
 	addObjetos();
 	renderFloor();
}

/**
 * Cria sequencia de rachaduras de acordo com a direcao jogador
 */
void criaRachadura(int x,int y,int direcao){
	int i;
	if(direcao == 3){
		for(i=x+1;i<20;i++)
			if(nivel1[i][y].ativo && nivel2[i][y].tipo == 'V') nivel2[i][y].metamorfisa('R',modelRach); else break;
	} else if(direcao == 1){
		for(i=x-1;i>=0;i--)
			if(nivel1[i][y].ativo && nivel2[i][y].tipo == 'V') nivel2[i][y].metamorfisa('R',modelRach); else break;
	} else if(direcao == 0) {
		for(i=y+1;i<20;i++)
			if(nivel1[x][i].ativo && nivel2[x][i].tipo == 'V') nivel2[x][i].metamorfisa('R',modelRach); else break;
	} else if(direcao == 2){
		for(i=y-1;i>=0;i--)
			if(nivel1[x][i].ativo && nivel2[x][i].tipo == 'V') nivel2[x][i].metamorfisa('R',modelRach); else break;
	}
	partida.aplicaCorte(nivel1[0],nivel2[0]);
}

void updateState() {
	cam.update();
	// Blocos no nivel 1 e 2
	int i,x,y;
	for(i=0;i<400;i++){
		x = i/20; y = i%20;
		nivel1[x][y].update();
		nivel2[x][y].update();
		if(nivel2[x][y].tipo != 'V' && !nivel1[nivel2[x][y].x][nivel2[x][y].y].ativo) // Verifica se elemento de nivel 2 está
			nivel2[x][y].emQueda = true;																								// em cima de um elemento válido do nível 1
	}
	// Jogaador
	jog.updateJogador(nivel2[0],&inimigos);
	if(!nivel1[jog.x][jog.y].ativo) 												// TODO: unificar com função de cima
		jog.emQueda = true;
	// TODO: colocar isso dentro da função verificaJogador() ou update()!!!!
	int xbur,ybur;
	if(spacePressed && !jog.isLock() && jog.emCimaBuraco(nivel2[0],&xbur,&ybur)){
		jog.setLock(0.03f);
		criaRachadura(xbur,ybur,jog.direcao);
	}
	// Inimigos
	for (it = inimigos.begin() ; it != inimigos.end(); ++it)
		it->second.updateInimigo(it->first,nivel1[0],nivel2[0],&jog,inimigos);

	if(emTeste){
		nivel1[rand()%20][rand()%20].emQueda = true;
	}

}

void animacaoFinal(){
	for(int i=0;i<400;i++){
		nivel1[i/20][i%20].tipo = 'V';
		nivel2[i/20][i%20].tipo = 'V';
	}
}

/**
 * Verifica final do jogo considerando quedas.
 * se todos inimigos cairam ou se o jogador caiu
 */
void verificaFimDeJogo(){
	if(jog.tipo == 'V'){ // Após animação de queda tipo é alterado para vazio
		partida.finalizada = true;
		partida.venceu = false;
		animacaoFinal();
	} else {
		bool allDead = true;
		for (it = inimigos.begin() ; it != inimigos.end(); ++it)
			if(it->second.tipo ==  'I')
				allDead = false;
		 if(allDead){
			partida.finalizada = partida.venceu = true;
			animacaoFinal();
		}
	}
}

/**
 * Tela de fim e jogo
 */
void renderFimDeJogo(){
	glClearColor(backgrundColor[0],backgrundColor[1],backgrundColor[2],backgrundColor[3]);
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpar o depth buffer
 	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();
	cam.visualizacao=3;
	cam.updatePosicao(jog);
 	renderFloor();
	if(partida.venceu)
		addObjetosNivel(4);
	else
		addObjetosNivel(3);
}


/**
Render scene
*/
void mainRender() {
	if(partida.finalizada){
		renderFimDeJogo();
	} else {
		updateState();
		renderScene();
		verificaFimDeJogo();
	}
	glFlush();
	glutPostRedisplay();
	usleep(0);
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
	// varix = (x - mouseLastX);
	// variy -= (y - mouseLastY);
	// mouseLastX = x;
	// mouseLastY = y;
}

/**
Key press event handler
*/
void onKeyDown(unsigned char key, int x, int y) {
	switch (key) {
		case 32: spacePressed = true; break;
		case 'v': cam.trocaPressed = true; break;
		case 'w': jog.andaPressed = true; break;
		case 's': jog.voltaPressed = true; break;
		case 'a': jog.giraEsqPressed = true; break;
		case 'd': jog.giraDirPressed = true; break;
		case 'f': jog.empurraPressed = true; break;

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
		case 32: spacePressed = false; break;
		case 'v': cam.trocaPressed = false; break;
		case 'w': jog.andaPressed = false; break;
		case 's': jog.voltaPressed = false; break;
		case 'a': jog.giraEsqPressed = false; break;
		case 'd': jog.giraDirPressed = false; break;
		case 'f': jog.empurraPressed = false; break;
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

void subRender(){
	glutSetWindow(subWindowId);
	glClearColor(backgrundColor[0],backgrundColor[1],backgrundColor[2],backgrundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

 	glMatrixMode(GL_MODELVIEW);
	cam.setViewAerea();

 	addObjetos();
 	renderFloor();

	glFlush();
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


	// Sub window
	subWindowId = glutCreateSubWindow(mainWindowId, 0, 0,(windowWidth/3), (windowHeight/3));
  glutDisplayFunc(subRender);
	subInit();

	glutMainLoop();

    return 0;
}
