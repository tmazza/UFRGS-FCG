#include "Bloco.h"

class Camera {
public:
  Camera();
  bool trocaPressed;
  float lock;
  int visualizacao;
  void update();
  void updatePosicao(Bloco jogador);
  void setViewAerea();
  bool setLock(float t);
  bool isLock();
};
