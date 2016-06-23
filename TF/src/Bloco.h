#ifndef BLOCO_HEADER
#define BLOCO_HEADER

class Bloco {
public:
  Bloco();
  int id;
  char tipo;
  int x,y;
  float posx,posy,posz;
  float gravidade;
  void *modelo;
  bool emQueda,ativo,inicializado;
  bool andaPressed,voltaPressed,giraEsqPressed,giraDirPressed;
  int direcao;
  float lock;

  void update();
  void verificaQueda();
  void verificaJogador(Bloco vizinho[20][20]);
  void controleDeQueda();
  void giraParaEsquerda();
  void giraParaDireita();
  bool isLock();
  bool setLock(float t);
  bool jogadorTemColisao(float x,float z,Bloco v[20][20]);
  bool pontoDentro(float x,float z);

  void render();
  void instanciar(char tipo,int x, int y, float posVertical, void *modelo);
  void resetPosicao(int x, int y, float posVertical);

};

#endif
