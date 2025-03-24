#include<stdio.h>
#include<stdlib.h>
#include<GL/glut.h>
#include<math.h>
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 //one degree in rad


typedef struct{
    int w,a,d,s;    //button state on|off
}ButtonKeys; ButtonKeys Keys;

//PLAYER SHYT
float degToRad(float a){return a*M_PI/180.0;}
float FixAng(float a){ if(a>359){a-=360;} if(a<0){a+=360;} return a;}

float px, py, pdy, pdx, pa ; ///for player position

void drawPlayer(){
    glColor3f(1,1,0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px, py);
    glEnd();

    glLineWidth(5);
    glBegin(GL_LINES);
    glVertex2i(px, py);
    glVertex2i(px+pdx*5, py+pdy*5);
    glEnd();
}

//ENVIRONMENT SHYT [MAP]
#define mapX 8      //map width
#define mapY 8      //map height
#define mapS 64    //cube size
int map[]=
{
    1,1,1,1,1,1,1,1,
    1,0,1,0,0,0,1,1,
    1,0,1,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,1,0,1,
    1,0,0,0,0,1,0,1,
    1,1,1,1,1,1,1,1,
};

void drawMap2D(){
    int x,y,xo,yo;
    for(y=0;y<mapY;y++){
        for(x=0;x<mapX;x++){
            if(map[y*mapX+x] == 1) glColor3f(1,1,1); else glColor3f(0,0,0);
            xo=x*mapS; yo=y*mapS;
            glBegin(GL_QUADS);
            glVertex2i(xo     +1, yo     +1);
            glVertex2i(xo     +1, yo+mapS-1);
            glVertex2i(xo+mapS-1, yo+mapS-1);
            glVertex2i(xo+mapS-1, yo     +1);
            glEnd();
        }
    }
}

//RAYCASTING SHYT

float dist(float ax, float ay, float bx, float by, float ang){
    return (sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)));
}

void drawRays2D()
{
 glColor3f(0,1,1); glBegin(GL_QUADS); glVertex2i(526,  0); glVertex2i(1006,  0); glVertex2i(1006,160); glVertex2i(526,160); glEnd();
 glColor3f(0,0,1); glBegin(GL_QUADS); glVertex2i(526,160); glVertex2i(1006,160); glVertex2i(1006,320); glVertex2i(526,320); glEnd();

 int r,mx,my,mp,dof,side; float vx,vy,rx,ry,ra,xo,yo,disV,disH;

 ra=FixAng(pa+30);                                                              //ray set back 30 degrees

 for(r=0;r<60;r++)
 {
  //---Vertical---
  dof=0; side=0; disV=100000;
  float Tan=tan(degToRad(ra));
       if(cos(degToRad(ra))> 0.001){ rx=(((int)px>>6)<<6)+64;      ry=(px-rx)*Tan+py; xo= 64; yo=-xo*Tan;}//lookinn left
  else if(cos(degToRad(ra))<-0.001){ rx=(((int)px>>6)<<6) -0.0001; ry=(px-rx)*Tan+py; xo=-64; yo=-xo*Tan;}//lookinn right
  else { rx=px; ry=py; dof=8;}                                                  //lookinn up or down. no hit

  while(dof<8)
  {
   mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
   if(mp>0 && mp<mapX*mapY && map[mp]==1){ dof=8; disV=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);}//hit
   else{ rx+=xo; ry+=yo; dof+=1;}                                               //check next horizontal
  }
  vx=rx; vy=ry;

  //---Horizontal---
  dof=0; disH=100000;
  Tan=1.0/Tan;
       if(sin(degToRad(ra))> 0.001){ ry=(((int)py>>6)<<6) -0.0001; rx=(py-ry)*Tan+px; yo=-64; xo=-yo*Tan;}//lookinnn up
  else if(sin(degToRad(ra))<-0.001){ ry=(((int)py>>6)<<6)+64;      rx=(py-ry)*Tan+px; yo= 64; xo=-yo*Tan;}//lookinnn down
  else{ rx=px; ry=py; dof=8;}                                                   //lookinn straight left or right

  while(dof<8)
  {
   mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
   if(mp>0 && mp<mapX*mapY && map[mp]==1){ dof=8; disH=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);}//hit
   else{ rx+=xo; ry+=yo; dof+=1;}                                               //check next horizontal
  }

  glColor3f(0,0.8,0);
  if(disV<disH){ rx=vx; ry=vy; disH=disV; glColor3f(0,0.6,0);}                  //horizontal hit first
  glLineWidth(2); glBegin(GL_LINES); glVertex2i(px,py); glVertex2i(rx,ry); glEnd();//draw 2D ray

  int ca=FixAng(pa-ra); disH=disH*cos(degToRad(ca));                            //fix fisheye
  int lineH = (mapS*320)/(disH); if(lineH>320){ lineH=320;}                     //line height and limit
  int lineOff = 160 - (lineH>>1);                                               //line offset

  glLineWidth(8);glBegin(GL_LINES);glVertex2i(r*8+530,lineOff);glVertex2i(r*8+530,lineOff+lineH);glEnd();//draw vertical wall

  ra=FixAng(ra-1);                                                              //go to next ray
 }
}

float frame1, frame2, fps;

void display(){
    //fps
    frame2 = glutGet(GLUT_ELAPSED_TIME); fps=(frame2-frame1); frame1 = glutGet(GLUT_ELAPSED_TIME);

    if (Keys.a == 1) { pa -= 0.1*fps; pa=FixAng(pa); pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa)); }
    if (Keys.d == 1) { pa += 0.1*fps; pa=FixAng(pa); pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa));  }
    if (Keys.w == 1) { px += pdx*0.1*fps; py += pdy*0.1*fps; }
    if (Keys.s == 1) { px -= pdx*0.1*fps; py -= pdy*0.1*fps; }
    glutPostRedisplay();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawRays2D();
    drawPlayer();
    glutSwapBuffers();
}

void init(){
    glClearColor(.3,.3,.3,0);
    gluOrtho2D(0,1024,512,0);
    px=300; py=300,pdx=cos(pa)*5; pdy=sin(pa)*5;
}

void ButtonDown(unsigned char key, int x, int y){
    if (key == 'a' || key == 'A') { Keys.a=1; }
    if (key == 'd' || key == 'D') { Keys.d=1; }
    if (key == 'w' || key == 'W') { Keys.w=1; }
    if (key == 's' || key == 'S') { Keys.s=1; }
}

void ButtonUp(unsigned char key, int x, int y){
    if (key == 'a' || key == 'A') { Keys.a=0; }
    if (key == 'd' || key == 'D') { Keys.d=0; }
    if (key == 'w' || key == 'W') { Keys.w=0; }
    if (key == 's' || key == 'S') { Keys.s=0; }
}

void SpecialButtonDown(unsigned char key, int x, int y){
    if (key == GLUT_KEY_LEFT)   { Keys.a=1; }
    if (key == GLUT_KEY_RIGHT)  { Keys.d=1; }
    if (key == GLUT_KEY_UP)     { Keys.w=1; }
    if (key == GLUT_KEY_DOWN)   { Keys.s=1; }
}

void SpecialButtonUp(unsigned char key, int x, int y){
    if (key == GLUT_KEY_LEFT)   { Keys.a=0; }
    if (key == GLUT_KEY_RIGHT)  { Keys.d=0; }
    if (key == GLUT_KEY_UP)     { Keys.w=0; }
    if (key == GLUT_KEY_DOWN)   { Keys.s=0; }
}

void resize(int w, int h){
    glutReshapeWindow(1024, 512);
}

int main(int argc, char* argv[]){

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1024, 512);
    glutInitWindowPosition(200,200);
    glutCreateWindow("MyWindow");
    init();
    glutDisplayFunc(display);
    // For 'a', 'w', 's', 'd'
    glutKeyboardFunc(ButtonDown);
    glutKeyboardUpFunc(ButtonUp);
    // For arrow keys
    glutSpecialFunc(SpecialButtonDown);
    glutSpecialUpFunc(SpecialButtonUp);

    glutReshapeFunc(resize);
    glutMainLoop();
    return 0;

}
