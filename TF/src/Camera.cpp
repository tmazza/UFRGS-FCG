#include <stdio.h>
#include <math.h>
#include "Bloco.h"
#include "Camera.h"
#include <GL/glut.h>

#define PI 3.14159265

Camera::Camera(){
  this->trocaPressed = false;
  this->visualizacao=0;
  this->lock = 0.0f;
}

void Camera::update(){
  if(this->trocaPressed && !this->isLock()){
    this->visualizacao+=1;
    this->visualizacao%=3;
    this->setLock(0.02);
  }
  this->setLock(this->lock - 0.001f);
}

void Camera::updatePosicao(Bloco jogador){
  float somax=0.0,somaz=0.0;
	if(jogador.direcao == 0) somaz=-1.0;
	if(jogador.direcao == 1) somax=+1.0;
	if(jogador.direcao == 2) somaz=+1.0;
	if(jogador.direcao == 3) somax=-1.0;
  if(this->visualizacao == 0) { // Em 2º pessoa
  	gluLookAt(jogador.posx+somax,jogador.posy+0.5,jogador.posz+somaz,
  		jogador.posx,jogador.posy,jogador.posz,
  		0.0,1.0,0.0);
  } else if(this->visualizacao == 1){ // Camera aerea
    gluLookAt(jogador.posx-(somax/10),jogador.posy+0.0,jogador.posz-(somaz/10),
      jogador.posx-somax,jogador.posy,jogador.posz-somaz,
      0.0,1.0,0.0);
  } else { // 1º pessoa
    gluLookAt(0.0,11.0,0.0,
      0.0,0.0,0.1,
      0.0,1.0,0.0);
  }
}

/**
 * Bloqueia atividade no bloco por 1000*t iterações.
 */
bool Camera::setLock(float t){
  this->lock = t;
  if(this->lock < 0.0f)
    this->lock = 0.0f;
}

/**
 * Controla se alguma ação está não finalziada no bloco.
 */
bool Camera::isLock(){
  return this->lock > 0.0f;
}
