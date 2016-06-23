/** Flood-fill (node, target-color, replacement-color):
 1. If target-color is equal to replacement-color, return.
 2. If the color of node is not equal to target-color, return.
 3. Set the color of node to replacement-color.
 4. Perform Flood-fill (one step to the south of node, target-color, replacement-color).
    Perform Flood-fill (one step to the north of node, target-color, replacement-color).
    Perform Flood-fill (one step to the west of node, target-color, replacement-color).
    Perform Flood-fill (one step to the east of node, target-color, replacement-color).
 5. Return. */

#include <stdio.h>

int m[20][20];

void FloodFillAlgorithm::flood(Point startPoint, Mat* tgtMat) {
    if (!insideImage(startPoint))
        return;

    Vec3b srcColor = image->at<Vec3b>(startPoint);

    if (image->at<Vec3b>(startPoint) == srcColor) {

    queue<Point> pointQueue;
  	pointQueue.push(startPoint);

  	while (!pointQueue.empty()) {
  	    Point p = pointQueue.front();
  	    pointQueue.pop();

  	    if (insideImage(p)) {

  		if ((image->at<Vec3b>(p) == srcColor)) {
  		    image->at<Vec3b>(p) = tgtMat->at<Vec3b>(p);

  		    pointQueue.push(Point(p.x + 1, p.y));
  		    pointQueue.push(Point(p.x - 1, p.y));
  		    pointQueue.push(Point(p.x, p.y + 1));
  		    pointQueue.push(Point(p.x, p.y - 1));
  		}
  	    }

  	}
  }
}

int main(){
  int i,j;

  for(i=0;i<20;i++)
    for(j=0;j<20;j++)
      m[i][j]=0;

  for(i=0;i<20;i++){
    for(j=0;j<20;j++){

    }
  }

  for(i=0;i<20;i++){
    for(j=0;j<20;j++){
      printf("%d",m[i][j]);
    }
    printf("\n");
  }



  return 0;
}
