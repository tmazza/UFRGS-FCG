#include <string.h>
#include <stdio.h>
#include <utility>
#include <queue>

using namespace std;

struct po { int x,y; };
typedef struct po PONTO;

int m[20][20];

/**
 * Opera em matriz 20x20 considerando zero comno cor de busca e 1 como nova cor.
 */
void flood(pair<int,int> p) {
  queue< pair<int,int> > pontos;
  pontos.push(p);
   while (!pontos.empty()) {
     pair<int,int> p = pontos.front();
     pontos.pop();
     if (p.first >= 0 && p.first < 20 && p.second >= 0 && p.second < 20) {
   		if (m[p.first][p.second] == 0) {
        m[p.first][p.second] = 1;
        pontos.push(make_pair(p.first+1,p.second  ));
        pontos.push(make_pair(p.first-1,p.second ));
        pontos.push(make_pair(p.first  ,p.second+1));
        pontos.push(make_pair(p.first  ,p.second-1));
  		}
    }
  }
}

int main(){
  for(int i=0;i<20;i++)
    for(int j=0;j<20;j++)
      m[i][j] = 0;

  for(int i=5;i<15;i++) { m[i][5]=2; m[i][15]=2; }
  for(int i=5;i<15;i++) { m[5][i]=2; m[15][i]=2; }

  flood(make_pair(10,10));

  for(int i=0;i<20;i++){
    for(int j=0;j<20;j++)
      printf("%d",m[i][j]);
    printf("\n");
  }
  return 0;
}
