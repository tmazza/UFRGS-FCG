
#include <map>

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
  bool emQueda,ativo,inicializado,andando;
  // Blocos com movimento
  bool andaPressed,voltaPressed,giraEsqPressed,giraDirPressed,empurraPressed;
  bool comTextura;
  int direcao,mudarDirecao;
  float lock,energiaArma;

  void update();
  void verificaQueda();
  void updateJogador(Bloco *n2,std::map<int,Bloco>* inimigos);
  void empurraInimigo(std::map<int,Bloco>* inimigos);
  void controleDeQueda();
  void giraParaEsquerda();
  void giraParaDireita();
  bool isLock();
  bool setLock(float t);
  bool jogadorTemColisao(float x,float z,Bloco *n2);
  bool pontoDentro(float x,float z, float raio);
  bool emCimaBuraco(Bloco *n2,int *x,int *y);

  void render();
  void instanciar(char tipo,int x, int y, float posVertical, void *modelo);
  void resetPosicao(int x, int y, float posVertical);

  void updateInimigo(int id,Bloco *n1,Bloco *n2,Bloco *jog,std::map<int,Bloco> inimigos);
  bool inimigoTemColisao(float x,float z,Bloco *n2);
  bool colisaoComOutroInimigo(int id,std::map<int,Bloco> inimigos);

  void metamorfisa(char tipo,void *modelo);

};

#endif
