#include <stdio.h>
#include <math.h>
#include "Camera.h"

#define PI 3.14159265

Camera::Camera(){
  this->frentePressed = this->trasPressed = this->upPressed = this->downtPressed = false;
  this->x = this->y = this->z = 6.0f;
  this->rotx = this->roty = 90.0f;
}

void Camera::update(){
  float speedBase = 0.05;
  if (this->frentePressed || this->trasPressed) {
    float speedX = +speedBase * sin(this->roty*PI/180) * 2;
    float speedZ = -speedBase * cos(this->roty*PI/180) * 2;
    if (this->frentePressed) {
        this->x += speedX;
        this->z += speedZ;
    } else if(this->trasPressed) {
        this->x -= speedX;
        this->z -= speedZ;
    }
  }
  if(this->upPressed){
    this->y += speedBase;
  } else if(this->downtPressed) {
    this->y -= speedBase;
  }
}
