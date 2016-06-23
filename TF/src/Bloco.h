class Bloco {
public:
  Bloco();
  int x,y;
  float posx,posy,posz;
  float gravidade;
  void *modelo;
  bool emQueda,visivel,inicializado;

  void update();
  void verificaQueda();

  void render();
  void instanciar(int x, int y, float posVertical, void *modelo);
  void resetPosicao(int x, int y, float posVertical);




};
