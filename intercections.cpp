#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

struct point {
	GLfloat x;
	GLfloat y;
} ;


GLfloat lerp(GLfloat p1, GLfloat p2,GLfloat t){
	GLfloat pf;
	pf = p1 + (p2 - p1) * t;
	return pf;
}

point* calcQLintersects(point p1, point p2, point p3, point a1, point a2) {
  point intersections[2];

	point normal, c2, c1, c0;


  // inverse line normal
  normal.x= a1.y-a2.y;
  normal.y= a2.x-a1.x;


  // Q-coefficients

  c2.x = p1.x + p2.x*-2 + p3.x;
  c2.y = p1.y + p2.y*-2 + p3.y;

  c1.x = p1.x*-2 + p2.x*2;
  c1.y = p1.y*-2 + p2.y*2;

  c0.x = p1.x;
  c0.y = p1.y;


  // Transform to line
  GLfloat coefficient=a1.x*a2.y-a2.x*a1.y;
  GLfloat a=normal.x*c2.x + normal.y*c2.y;
  GLfloat b=(normal.x*c1.x + normal.y*c1.y)/a;
  GLfloat c=(normal.x*c0.x + normal.y*c0.y + coefficient)/a;

  // solve the roots
  GLfloat roots[2]= {NULL, NULL};
  GLfloat d = b*b-4*c;
  if(d>0){
    roots[0] = (-b+sqrt(d))/2;
    roots[1] = (-b-sqrt(d))/2;
  }else if(d==0){
    roots[0] = -b/2;
  }

  // calc the solution points
  for(int i=0;i<2;i++){
    if (roots[i]!=NULL){
      GLfloat minX = fmin(a1.x,a2.x);
      GLfloat minY = fmin(a1.y,a2.y);
      GLfloat maxX = fmax(a1.x,a2.x);
      GLfloat maxY = fmax(a1.y,a2.y);
      GLfloat t = roots[i];
      if (t>=0 && t<=1) {
        // possible point

        point i_point;
        i_point.x = lerp(lerp(p1.x,p2.x,t),lerp(p2.x,p3.x,t),t);
        i_point.y = lerp(lerp(p1.y,p2.y,t),lerp(p2.y,p3.y,t),t);

        // bounds checks
        if(a1.x==a2.x && i_point.y>=minY && i_point.y<=maxY){
          // vertical line
          intersections[i]=i_point;
        }else if(a1.y==a2.y && i_point.x>=minX && i_point.x<=maxX){
          // horizontal line
          intersections[i]=i_point;
        }else if(i_point.x>=minX && i_point.y>=minY && i_point.x<=maxX && i_point.y<=maxY){
          // line passed bounds check
          intersections[i]=i_point;
        }
      }
    }
  }
  return intersections;
}
