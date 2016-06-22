#include <stdio.h>
#include "Camera.h"

Camera::Camera(){
  this->frentePressed = this->trasPressed = this->upPressed = this->downtPressed = false;
  this->x = this->y = this->z = 6.0f;
}

// void update(){
//   float speedBase = 0.05;
//   if (this->frentePressed || this->trasPressed) {
//     float speedX = +speedBase * sin(roty*PI/180) * 2;
//     float speedZ = -speedBase * cos(roty*PI/180) * 2;
//     if (this->frentePressed) {
//         posX += speedX;
//         posZ += speedZ;
//     } else if(this->trasPressed) {
//         posX -= speedX;
//         posZ -= speedZ;
//     }
//   }
//   if(this->upPressed){
//     posY += speedBase;
//   } else if(this->downtPressed) {
//     posY -= speedBase;
//   }
// }
