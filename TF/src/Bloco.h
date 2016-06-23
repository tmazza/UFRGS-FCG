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
  bool andaPressed,voltaPressed,giraEsqPressed,giraDirPressed;
  int direcao;
  float lock;

  void update();
  void verificaQueda();
  void verificaJogador();
  void giraParaEsquerda();
  void giraParaDireita();
  bool isLock();
  bool setLock(float t);

  void render();
  void instanciar(char tipo,int x, int y, float posVertical, void *modelo);
  void resetPosicao(int x, int y, float posVertical);

  void incX(float step);

};

#endif
