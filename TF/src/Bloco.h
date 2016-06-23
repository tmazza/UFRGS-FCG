#ifndef BLOCO_HEADER
#define BLOCO_HEADER

class Bloco {
public:
  Bloco();
  char tipo;
  int x,y;
  float posx,posy,posz;
  float gravidade;
  void *modelo;
  bool emQueda,visivel,inicializado;

  void update();
  void verificaQueda();

  void render();
  void instanciar(char tipo,int x, int y, float posVertical, void *modelo);
  void resetPosicao(int x, int y, float posVertical);
};

#endif
