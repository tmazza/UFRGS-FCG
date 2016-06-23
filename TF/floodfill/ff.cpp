#include <string.h>
#include <stdio.h>
#include <queue>

using namespace std;

struct po { int x,y; };
typedef struct po PONTO;

int m[20][20];

int srcColor = 0;

bool insideImage(PONTO p) {
    return p.x >= 0 && p.x < 20 && p.y >= 0 && p.y < 20;
}
void flood(PONTO p) {
    queue<PONTO> pointQueue;
    pointQueue.push(p);
  	while (!pointQueue.empty()) {
      PONTO p = pointQueue.front();
      pointQueue.pop();
      if (insideImage(p)) {
    		if (m[p.x][p.y] == srcColor) {
    		    m[p.x][p.y] = 1;

            PONTO p1,p2,p3,p4;
            p1.x = p.x + 1;
            p1.y = p.y;
            p2.x = p.x - 1;
            p2.y = p.y;

            p3.x = p.x;
            p3.y = p.y + 1;
            p4.x = p.x;
            p4.y = p.y - 1;

    		    pointQueue.push(p1);
    		    pointQueue.push(p2);
    		    pointQueue.push(p3);
    		    pointQueue.push(p4);
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

  PONTO start;
  start.x = 10;
  start.y = 10;
  flood(start);


  for(int i=0;i<20;i++){
    for(int j=0;j<20;j++)
      printf("%d",m[i][j]);
    printf("\n");
  }
  return 0;
}
