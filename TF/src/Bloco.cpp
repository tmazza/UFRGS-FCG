#include <stdio.h>
#include "Bloco.h"
#include <GL/glut.h>
#include "glm.h"

/**
 * Tipos possiveis | representacao no bitmap em (TGB)
 * J: jogador | 001
 * V: vazio | 011
 * I: inimigo | 100
 * P: parede (ou bloco conforme especificação) | 010
 * B: buraco | 110
 * R: rachadura | 101
 */
Bloco::Bloco(){
  this->emQueda = false;
  this->inicializado = false;
  this->visivel = true;
  this->gravidade = 0.0025;
}

void Bloco::render(){
  if(this->tipo != 'V' && this->visivel){
    glPushMatrix();
    glTranslatef(this->posx,this->posy,this->posz);
    glmDraw((GLMmodel *) this->modelo, GLM_SMOOTH);
    glPopMatrix();
  }
}

/**
 * Evento aplicados no bloco
 */
void Bloco::update(){
  this->verificaQueda();
}

/**
 * Caso objeto esteja em queda, realiza animação e esconde bloco;
 */
void Bloco::verificaQueda(){
  if(this->emQueda && this->posy > -2.0){
    this->posy -= this->gravidade; // Linear :D
  } else if(this->posy <= -2.0){
    this->visivel = false;
  }
}

/**
 * Cria bloco na posicao esperada. Realiza operação somente uma vez,
 * quando o objeto ainda nao esta inicializado
 */
void Bloco::instanciar(char tipo,int x, int y, float posVertical, void *modelo){
  if(!this->inicializado){
    this->tipo = tipo;
    this->modelo = (GLMmodel *) modelo;
    this->resetPosicao(x,y,posVertical);
    this->inicializado = true;
  }
}

/**
 * Par x,y relativo a matrix do cenario
 * posVertical relativa a posy
 */
void Bloco::resetPosicao(int x, int y, float posVertical){
  if(!this->emQueda){
    this->x = x;
    this->y = y;
    this->posx = x*0.4-4.0;
    this->posy = posVertical;
    this->posz = y*0.4-4.0;
  }
}
