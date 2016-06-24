#include <utility>
class Partida {
public:
  // Partida();
  int aux[20][20];
  void aplicaCorte(Bloco *n1,Bloco *n2);
  void flood(std::pair<int,int> p,int novoCor);
};
