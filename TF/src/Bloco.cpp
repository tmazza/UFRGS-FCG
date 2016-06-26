#include <stdio.h>
#include <utility>
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
  this->andando = true;
  this->gravidade = 0.01;
  this->comTextura = false;
  this->lock = 0.0f;

  // Usado para o jogador
  this->andaPressed = this->voltaPressed = this->giraEsqPressed = this->giraDirPressed = this->empurraPressed = false;
  this->direcao = 0; // 0=norte|1=oeste|2=sul|3=lest | sentido horario

  this->mudarDirecao = 0;
}

void Bloco::render(){
  if(this->tipo != 'V'){
    glPushMatrix();
    glTranslatef(this->posx,this->posy,this->posz);

    if(tipo == 'R' || this->tipo == 'B'){
      this->posy=0.801;
    }

    if(this->tipo == 'R' || this->tipo == 'P' || this->tipo == 'B'){
      glmDraw((GLMmodel *) this->modelo, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
    } else {
      glmDraw((GLMmodel *) this->modelo, GLM_SMOOTH);
    }
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
  if(this->emQueda && this->posy > -1.0){
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
void Bloco::updateJogador(Bloco n2[20][20],std::map<int,Bloco>* inimigos){
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
  if(!jogadorTemColisao(novoX,novoZ,n2)){
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

  if(this->empurraPressed && this->energiaArma > 0.0){
    empurraInimigo(inimigos);
    this->energiaArma -= 0.1;
  } else if(this->energiaArma < 5.0){
    this->energiaArma += 0.003;
  }

  this->x = (this->posx+4.2)/0.4;
  this->y = (this->posz+4.2)/0.4;


}

/**
 * Verifica se tem um inimigo a até 2 casas de distância e empurra para
 * posicao do jogador + duas casas
 */
void Bloco::empurraInimigo(std::map<int,Bloco>* inimigos){
  std::map<int,Bloco>::iterator it;
  std::pair< float, float > p1;
  std::pair< float, float > p2;
  // A primeira e a segunda casa a frente do jogador
  if(this->direcao == 0){ p1 = std::make_pair(this->posx,this->posz+0.4); p2 = std::make_pair(this->posx,this->posz+0.8); }
  if(this->direcao == 1){ p1 = std::make_pair(this->posx-0.4,this->posz); p2 = std::make_pair(this->posx-0.8,this->posz); }
  if(this->direcao == 2){ p1 = std::make_pair(this->posx,this->posz-0.4); p2 = std::make_pair(this->posx,this->posz-0.8); }
  if(this->direcao == 3){ p1 = std::make_pair(this->posx+0.4,this->posz); p2 = std::make_pair(this->posx+0.8,this->posz); }

  float incx=0.0,incz=0.0;
  for(it = inimigos->begin();it!=inimigos->end();++it){
    if(it->second.pontoDentro(p1.first,p1.second,0.4)
       || it->second.pontoDentro(p2.first,p2.second,0.4)){
      if(this->direcao == 0){ incz = +0.1; }
      if(this->direcao == 1){ incx = -0.1; }
      if(this->direcao == 2){ incz = -0.1; }
      if(this->direcao == 3){ incx = +0.1; }
      it->second.posx += incx;
      it->second.posz += incz;
    }
  }
}

/**
 * Realiza movimentação de bloco do tipo inimigo
 */
void Bloco::updateInimigo(int id,Bloco n1[20][20],Bloco n2[20][20],Bloco* jog,std::map<int,Bloco> inimigos){
  this->update();
  if(this->ativo && this->andando){
    float step = 0.004f;
    float novoPosX = this->posx,novoPosZ = this->posz;
    int novox,novoy;

    if(this->mudarDirecao == 0 && jog->pontoDentro(this->posx,this->posz,1.8)){
      if(jog->y > this->y){
        this->direcao = jog->x > this->x ? 0 : 1;
      } else {
        this->direcao = jog->x > this->x ? 3 : 2;
      }
    } else if (this->mudarDirecao != 0) {
      this->mudarDirecao == 1 ? this->giraParaEsquerda() : this->giraParaDireita();
      this->mudarDirecao = 0;
    }

    if(this->direcao == 0) novoPosZ += step;
    if(this->direcao == 1) novoPosX -= step;
    if(this->direcao == 2) novoPosZ -= step;
    if(this->direcao == 3) novoPosX += step;

    novox = (novoPosX+4.2)/0.4;
    novoy = (novoPosZ+4.2)/0.4;

    int random = rand() % 256;
    if(random == 1 || random == 2
      || n1[novox][novoy].tipo == 'V' // Buraco no nivel 1
      || novoPosZ > 3.8f || novoPosZ < -4.2f || novoPosX > 3.8f || novoPosX < -4.2f // Limites do mapa
      || this->inimigoTemColisao(novoPosX,novoPosZ,n2) // Colisão com bloco
    ){
      this->mudarDirecao = random == 1 ? 1 : 2;
    } else if(this->colisaoComOutroInimigo(id,inimigos)) {
      this->mudarDirecao = random == 1 ? 1 : 2;
      this->andando = false;
    } else {
      this->posx = novoPosX;
      this->posz = novoPosZ;
      this->x = novox;
      this->y = novoy;
    }

    if(!n1[this->x][this->y].ativo)
      this->emQueda = true;
  } else if(!this->andando){
    this->andando = true;
  }
  if(this->ativo) {
    if(jog->pontoDentro(this->posx,this->posz,0.36))    // Testa colisão com jogador
      jog->tipo = 'V';
  }
}

/**
 * Colisao entre si
 */
bool Bloco::colisaoComOutroInimigo(int id,std::map<int,Bloco> inimigos){
  std::map<int,Bloco>::iterator it;
  for(it = inimigos.begin() ;it != inimigos.end();++it)
    if(it->first > id && it->second.pontoDentro(this->posx,this->posz,0.4)){
      it->second.andando = false;
      return true;
    }
  return false;
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
        return true;
  return false;
}

/**
 * Verifica se existe colisão com algum elemento do cenario
 */
bool Bloco::inimigoTemColisao(float x,float z,Bloco n2[20][20]){
  int i,j;
  for(i=0;i<20;i++)
    for(j=0;j<20;j++)
      if(n2[i][j].id != this->id && (n2[i][j].tipo == 'P' || n2[i][j].tipo == 'B' || n2[i][j].tipo == 'R') && n2[i][j].pontoDentro(x,z,0.38))
        return true;
  return false;
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
