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
  this->ativo = true;
  this->gravidade = 0.01;
  this->lock = 0.0f;

  // Usado para o jogador
  this->andaPressed = this->voltaPressed = this->giraEsqPressed = this->giraDirPressed = false;
  this->direcao = 0; // 0=norte|1=oeste|2=sul|3=lest | sentido horario

}

void Bloco::render(){
  if(this->tipo != 'V'){
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
  if(this->tipo != 'V'){
    this->verificaQueda();
    this->controleDeQueda();
    this->setLock(this->lock - 0.001f);
  }
}

/**
 * Caso objeto esteja em queda, realiza animação e esconde bloco;
 */
void Bloco::verificaQueda(){
  if(this->emQueda && this->posy > -2.0){
    this->posy -= this->gravidade; // Linear :D
    this->ativo = false;
  } else if(this->posy <= -1.0){
    this->emQueda = false;
    this->tipo = 'V'; // Vazio nao é renderizado | perde-se a informação de tipo
  }
}

/**
 * Controle movimentos e interações do jogados
 */
void Bloco::verificaJogador(Bloco vizinho[20][20]){
	this->update();
  float novoX = this->posx,novoZ = this->posz;
  float step = 0.01f;
  if(this->andaPressed){
    if(this->direcao == 0) novoZ += step;
    if(this->direcao == 1) novoX -= step;
    if(this->direcao == 2) novoZ -= step;
    if(this->direcao == 3) novoX += step;
  } else if(this->voltaPressed){
    if(this->direcao == 0) novoZ -= step;
    if(this->direcao == 1) novoX += step;
    if(this->direcao == 2) novoZ += step;
    if(this->direcao == 3) novoX -= step;
  }
  if(jogadorTemColisao(novoX,novoZ,vizinho)){
    this->posx = novoX;
    this->posz = novoZ;
  }
  if (this->giraEsqPressed && !this->isLock()) {
    this->giraParaEsquerda();
    setLock(0.03);
  } else if(this->giraDirPressed && !this->isLock()){
    this->giraParaDireita();
    setLock(0.03);
  }
  this->x = (this->posx+4.2)/0.4;
  this->y = (this->posz+4.2)/0.4;

}

/**
 * Verifica colisão com buraco
 */
bool Bloco::emCimaBuraco(Bloco v[20][20],int *x,int *y){
  int i,j;
  for(i=0;i<20;i++)
    for(j=0;j<20;j++)
      if(v[i][j].id != this->id && v[i][j].tipo == 'B' && v[i][j].pontoDentro(this->posx,this->posz,0.2)){
        *x = v[i][j].x;
        *y = v[i][j].y;
        return true;
      }
  return false;
}

/**
 * Verifica se existe colisão com algum elemento do cenario
 */
bool Bloco::jogadorTemColisao(float x,float z,Bloco v[20][20]){
  int i,j;
  for(i=0;i<20;i++)
    for(j=0;j<20;j++)
      if(v[i][j].id != this->id && (v[i][j].tipo == 'P' || v[i][j].tipo == 'I') && v[i][j].pontoDentro(x,z,0.38))
        return false;
  return true;
}

/**
 * Verifica se ponto (x,z) está dentro de quadrado de lado raio*2
 */
bool Bloco::pontoDentro(float x,float z, float raio){
  return x < this->posx+raio && x > this->posx-raio && z < this->posz+raio && z > this->posz-raio;
}

/**
 * Verifica se jogador esta fora da região do mapa ou sob uma região vazia
 */
void Bloco::controleDeQueda(){
  if(this->posz > 3.8f || this->posz < -4.2f || this->posx > 3.8f || this->posx < -4.2f)  // Queda nas bordas
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
 * Bloqueia atividade no bloco por 1000*t iterações.
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
    if(tipo == 'V')
      this->ativo = false;
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
 * Muda tipo e modelo do bloco
 */
void Bloco::metamorfisa(char tipo,void *modelo){
  this->tipo = tipo;
  this->modelo = (GLMmodel *) modelo;
}
