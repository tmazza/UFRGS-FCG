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
  this->gravidade = 0.01;
  this->lock = 0.0f;

  // Usado para o jogador
  this->andaPressed = this->voltaPressed = this->giraEsqPressed = this->giraDirPressed = false;
  this->direcao = 0; // 0=norte|1=oeste|2=sul|3=lest | sentido horario

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
  if(this->tipo == 'J')
    this->verificaJogador();

  this->controleDeQueda();

  this->setLock(this->lock - 0.001f);
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
 * Controle movimentos e interações do jogados
 */
void Bloco::verificaJogador(){
  if(this->andaPressed){
    if(this->direcao == 0) this->posz += 0.01f;
    if(this->direcao == 1) this->posx -= 0.01f;
    if(this->direcao == 2) this->posz -= 0.01f;
    if(this->direcao == 3) this->posx += 0.01f;
  } else if(this->voltaPressed){
    if(this->direcao == 0) this->posz -= 0.01f;
    if(this->direcao == 1) this->posx += 0.01f;
    if(this->direcao == 2) this->posz += 0.01f;
    if(this->direcao == 3) this->posx -= 0.01f;
  }
  if (this->giraEsqPressed && !this->isLock()) {
    this->giraParaEsquerda();
    setLock(0.07);
  } else if(this->giraDirPressed && !this->isLock()){
    this->giraParaDireita();
    setLock(0.07);
  }
  this->x = (this->posx+4)/0.4;
  this->y = (this->posz+4)/0.4;
  // printf("%f - %f \n",this->posx,this->posz);
}

/**
 * Verifica se jogador esta fora da região do mapa ou sob uma região vazia
 */
void Bloco::controleDeQueda(){
  if(this->posz > 4.0f || this->posz < -4.4f || this->posx > 4.0f || this->posx < -4.4f)  // Queda nas bordas
    this->emQueda = true;

}

/**
* Muda direção para esquerda
*/
void Bloco::giraParaEsquerda(){
  this->direcao -= 1;
  if(this->direcao == -1)
    this->direcao = 3;
}

/**
 * Muda direção para direita
 */
void Bloco::giraParaDireita(){
  this->direcao += 1;
  if(this->direcao == 4)
    this->direcao = 0;
}

/**
 * Controla se alguma ação está não finalziada no bloco.
 */
bool Bloco::isLock(){
  return this->lock > 0.0f;
}

/**
 * Bloquei atividade no bloco por 1000*t iterações.
 */
bool Bloco::setLock(float t){
  this->lock = t;
  if(this->lock < 0.0f)
    this->lock = 0.0f;
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

/**
 * Incrementa x
 */
void Bloco::incX(float step){
  printf("asdad%f\n",step);
}
