#include <stdio.h>
#include <utility>
#include <queue>
#include "Bloco.h"
#include "Partida.h"

#define COR_BUSCA 0
#define COR_BLOCO 1
#define COR_NOVA1 2
#define COR_NOVA2 3

void Partida::aplicaCorte(Bloco *n1,Bloco *n2){
  int x,y;
  bool findStart = false;
  for(int i=0;i<400;i++){
    if((n1+i)->tipo != 'V' && (n2+i)->tipo != 'R' && (n2+i)->tipo != 'B'){
      this->aux[i/20][i%20] = COR_BUSCA;
      if(!findStart){
        x = i/20; y = i%20;
        findStart = false;
      }
    } else {
      this->aux[i/20][i%20] = COR_BLOCO;
    }
  }
  this->flood(std::make_pair(x,y),COR_NOVA1);

  // Verifica se alguma posição nao foi coberta
  findStart = false;
  int areaCoberta = 0;
  for(int i=0;i<400;i++){
    if(this->aux[i/20][i%20] == COR_BUSCA){
      findStart = true;
      x = i/20; y = i%20;
    } else if(this->aux[i/20][i%20] == COR_NOVA1){
      areaCoberta++;
    }
  }
  if(findStart){ // Uma parte nao foi cobeta
    // Busca na regiao desconexa
    this->flood(std::make_pair(x,y),COR_NOVA2);
    int areaCoberta2 = 0;
    int corParaRemover;
    for(int i=0;i<400;i++)
      if(this->aux[i/20][i%20] == COR_NOVA2)
        areaCoberta2++;

    corParaRemover = areaCoberta2 < areaCoberta ? COR_NOVA2 : COR_NOVA1;

    for(int i=0;i<400;i++){
      if(this->aux[i/20][i%20] == corParaRemover){
        (n1+i)->emQueda = true;
      }
    }
  }

}

/**
 * Opera em matriz 20x20 considerando zero comno cor de busca e 1 como nova cor.
 */
void Partida::flood(std::pair<int,int> p,int novoCor) {
  std::queue< std::pair<int,int> > pontos;
  pontos.push(p);
   while (!pontos.empty()) {
     std::pair<int,int> p = pontos.front();
     pontos.pop();
     if (p.first >= 0 && p.first < 20 && p.second >= 0 && p.second < 20) {
   		if (this->aux[p.first][p.second] == 0) {
        this->aux[p.first][p.second] = novoCor;
        pontos.push(std::make_pair(p.first+1,p.second  ));
        pontos.push(std::make_pair(p.first-1,p.second ));
        pontos.push(std::make_pair(p.first  ,p.second+1));
        pontos.push(std::make_pair(p.first  ,p.second-1));
  		}
    }
  }
}
