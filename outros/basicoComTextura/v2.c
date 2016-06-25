#include <GL/glut.h>
#include <stdio.h>

float angulo = 0.0f,rotX = 1.0,rotY = 0.0f,rotZ = 0.0f,velocidade = -1.00;
int w,h;

GLuint texid1;

char *load_bmp(char * name){

    FILE *f = fopen(name,"rb");
    if(f == NULL) exit(0);
    int of;
    fseek(f, 10, SEEK_SET);
    fread(&of, sizeof(int), 1, f);
    fseek(f, 4, SEEK_CUR);
    fread(&w, sizeof(int), 1, f);
    fread(&h, sizeof(int), 1, f);

    fseek(f, of, SEEK_SET);

    int by = ((w * 3 + 3)/4) * 4 - (w * 3 % 4);
    char *tmp_m = malloc(sizeof(char) * by * h);
    char *m = malloc(sizeof(char) * w * h * 3);
    fread(tmp_m, sizeof(char) * by * h, 1, f);
    int x,y,i;
    for(y = 0; y < h; y++){
        for(x = 0; x < w; x++){
            for(i = 0; i < 3; i++){
                m[3*(w*y + x) + i] = tmp_m[3*(w*y + x) + (2 - i)];
            }
        }
    }
    free(tmp_m);
    return m;
}

GLuint loadTex(char *c){
        char *wa = load_bmp(c);
        GLuint texid;
        glGenTextures(1, &texid);
        glBindTexture(GL_TEXTURE_2D, texid);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, wa);
        free(wa);
        return texid;
}

void colocaImagem(){
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texid1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

void DesenhaNaTela(void)
{

    colocaImagem();

    // Clear Color and Depth Buffers
    glClear(GL_COLOR_BUFFER_BIT);

    // Reset transformations
    glLoadIdentity();

    gluLookAt(  0.0f, 0.0f, 10.0f,
                        0.0f, 0.0f,  0.0f,
                        0.0f, 1.0f,  0.0f);

    glRotatef(angulo,rotX, rotY, rotZ);

        glBegin(GL_QUADS);

        glTexCoord3f(-1,1,0);glVertex3f(-1, 1, 0);
        glTexCoord3f(1,1,0);glVertex3f( 1, 1, 0);
        glTexCoord3f(1,-1,0); glVertex3f( 1,-1, 0);
        glTexCoord3f(-1,-1,0);  glVertex3f(-1,-1, 0);

        glEnd();

    angulo+=velocidade;

    glutSwapBuffers();
}

void Inicializa (void)
{
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        texid1 = loadTex("Textura.bmp");
}

void Teclado(unsigned char key, int x, int y){
    if(key == 27) exit(0); // O Programa Fecha Caso o Esc seja apertado ...

    if((char)key == ' '){
        if(rotX == 1.0f){
        rotX = 0.0f;
        rotY = 1.0f;
        rotZ = 0.0f;
        }else if (rotY == 1.0f){
        rotX = 0.0f;
        rotY = 0.0f;
        rotZ = 1.0f;
        }else if(rotZ == 1.0f){
        rotX = 1.0f;
        rotY = 0.0f;
        rotZ = 0.0f;
        }

    }

}

void TeclaEspeciais(int key, int x, int y){

    if(key == GLUT_KEY_UP) {
    velocidade += 0.05;
    printf("Velocidade Rotação : %.2f\n",velocidade);
    }
    if(key == GLUT_KEY_DOWN) {
    velocidade -=  0.05;
    printf("Velocidade Rotação : %.2f\n",velocidade);
    }

    if(key == GLUT_KEY_LEFT)
        if(velocidade < 0)velocidade *= -1;
    if(key == GLUT_KEY_RIGHT)
        if(velocidade > 0)velocidade *= -1;
}

void CliqueMouse(int button, int state, int x, int y){

    if(button == GLUT_LEFT_BUTTON){
        if(state == GLUT_DOWN){
        printf("Clicou com o botão esquerdo na Posição  X : %i Y : %i \n",x,y);
        }else if (state == GLUT_UP){
        printf("Soltou Botão esquerdo\n");
        }
    }else if(button == GLUT_RIGHT_BUTTON){
        if(state == GLUT_DOWN){
        printf("Clicou com o botão direito na Posição  X : %i Y : %i \n",x,y);
        }else if(state == GLUT_UP){
        printf("Soltou Botão direito\n");
        }
    }else if (button == GLUT_MIDDLE_BUTTON){
        if(state == GLUT_DOWN){
        printf("Clicou com o botão do meio na Posição  X : %i Y : %i \n",x,y);
        }else if(state == GLUT_UP){
        printf("Soltou Botão do meio\n");
        }
    }
}

void MovimentoMouse (int x,int y) {

    printf("Mouse na Posição  X : %i Y : %i \n",x,y);

}

void AlteraTamanhoTela(int w, int h) {

// Função é chamada caso a tela tenha tido alterada

    if (h == 0)
        h = 1;

    float proporsao =  w * 1.0 / h;

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glViewport(0, 0, w, h);

    gluPerspective(45.0f, proporsao, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(320,240);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Tutorial Imagens");
        Inicializa();
    glutDisplayFunc(DesenhaNaTela);
    glutIdleFunc(DesenhaNaTela);
    glutReshapeFunc(AlteraTamanhoTela);
    glutMouseFunc(CliqueMouse);
    glutPassiveMotionFunc(MovimentoMouse);
    glutKeyboardFunc( Teclado );
    glutSpecialFunc( TeclaEspeciais);
    glutMainLoop();
} 
