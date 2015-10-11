#include "GL/glut.h"
#include "GL/gl.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string"
#include "vector"
#include "ctime"
#include "iostream"

using namespace std;

void setChips(int);
void renderFunction();
void checkScore();
float distance(float,float,float,float);
void passive(int,int);
void handleMouse(int, int, int, int);

int boost = 1, proj_theta=90,score=30, start=-1, mark=0,w,h;
int red=0,black=0,white=0,strike=0;
time_t begin = time(0);
   

void drawCircle(double x, double y, int radius){
    glBegin(GL_POLYGON);
        for(int i=0;i<1000;++i){
          glVertex3f(radius*cos(2*M_PI*i/1000.0) + x,radius*sin(2*M_PI*i/1000.0) + y,0);
        }
    glEnd();
}

class chip{
public:
    float x_pos;
    float y_pos;
    float x_vel;
    float y_vel;
    int c_radius;
    int val;
    void init(int x, int y, int vx, int vy, int v){
        x_pos=x;
        y_pos=y;
        x_vel=vx;
        y_vel=vy;
        if(v==-1)
        	c_radius=15;
        else
        	c_radius=12;
        val=v;
    }
     void set(){
        if(val==-5)
            glColor3f(0, 0, 0);
        else if(val==10)
            glColor3f(1, 1, 1);
        else if(val==50)
            glColor3f(0.71, 0.03, 0.03);
        else{
            if(x_vel ==0 && y_vel ==0 && y_pos==-2*h/7){
                glTranslatef(x_pos,y_pos, 0);
                glRotatef(proj_theta, 0, 0, 1);
                glColor3f(0.71, 0.03, 0.03);
                glBegin(GL_TRIANGLE_STRIP);
                glVertex3f(20, 0, 0);
                glVertex3f(0, -10, 0);
                glVertex3f(0, 10, 0);
                glEnd();
                glRotatef(-proj_theta, 0, 0, 1);
                glTranslatef(-x_pos,-y_pos,0);
            }
            glColor3f(0.13, 0.37, 1.0);
        }
        drawCircle(x_pos, y_pos, c_radius);
        relocate();
     }
     void relocate(){
        x_pos += x_vel;
        y_pos += y_vel;
        if(fabs(x_vel) >0)
            x_vel = x_vel - 0.02*x_vel;
        if(fabs(y_vel) >0)
            y_vel = y_vel - 0.02*y_vel;
        if(fabs(x_vel)< 0.05)
            x_vel=0;
        if (fabs(y_vel)< 0.05)
            y_vel=0;
        if(fabs(x_pos) > 25*h/70)
            x_vel *= -1;
        if(fabs(y_pos) > 25*h/70)
            y_vel *= -1;
        if(val == -1 && x_vel==0 && y_vel ==0 && y_pos !=-2*h/7)
            init(10, -2*h/7, 0, 0, -1);
     }
};
chip striker,coin[19];

void instructions(){
    glClearColor(0.937, 0.780, 0.619,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    vector<string> text;
    text.push_back("INSTRUCTIONS:");
    text.push_back("  -> White scores 10 and black scores -5");
    text.push_back("  -> Red scores 50 ");
    text.push_back("  -> Striker carries -1");
    text.push_back("  -> The red mark on the striker points the sirection of the striker");
    text.push_back("  -> Initial score is 30 and score reduces by 1 every second");
    text.push_back("  -> Game ends when all white are cleared off the board");
    text.push_back("  -> Keyboard Controls:-");
    text.push_back("               -> Adjust striker position               : Left and Right Arrow keys");
    text.push_back("               -> Adjust Striker direction             : A and C keys");
    text.push_back("               -> Adjust initial power on striker   : up and down Arrow keys");
    text.push_back("               -> Strike                                          : Spacebar");
    text.push_back("  -> Mouse Controls:-");
    text.push_back("               -> Adjust striker position               : Right click");
    text.push_back("               -> Adjust Striker direction             : Inverted Mouse");
    text.push_back("               -> Adjust initial power on striker   : Distance of mouse from striker");
    text.push_back("               -> Strike                                          : Left click");   
    text.push_back("");
    text.push_back("");
    text.push_back("                       To start game Hit \"Enter\"");   
    for(int i=0;i<20;i++){
        char*c;
        glRasterPos3f(-400, 300-30*i,0);
        for (c=&text[i][0]; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
        }
    }
    glPopMatrix();
}

void GameOver(){
    glClearColor(0.937, 0.780, 0.619,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    vector<string> text;
    char str[100];
    text.push_back("ThankYou for playing");
    sprintf(str,"  White coins backed      : %d",white);
    text.push_back(str);
    sprintf(str,"  Black coins backed       : %d",black);
    text.push_back(str);
    sprintf(str,"  Red coins                      : %d",red);
    text.push_back(str);
    sprintf(str,"  Strikers                         : %d",strike);
    text.push_back(str); 
    sprintf(str,"  Final Score                    : %d",score);
    text.push_back(str);
    text.push_back("");
    text.push_back("");
    text.push_back("");
    text.push_back("                       To start new game Hit \"Enter\"");   
    for(int i=0;i<10;i++){
        char*c;
        glRasterPos3f(-100, 100-30*i,0);
        for (c=&text[i][0]; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
        }
    }
    glPopMatrix();
}

void drawFooter(int x, int y,int theta){
	glTranslatef(x, y,0);
    glRotatef(theta,0,0,1);
	glBegin(GL_LINE_LOOP);
		glVertex3f(-185.0, 11.0, 0.0);
    	glVertex3f(185.0, 11.0, 0.0);
        glVertex3f(196.0, 0.0, 0.0);
    	glVertex3f(185.0, -11.0, 0.0);
    	glVertex3f(-185.0, -11.0, 0.0);
        glVertex3f(-196.0, 0.0, 0.0);
    glEnd();
    glPushMatrix();
    drawCircle(-185, 0.0, 11.0);
    drawCircle(185, 0.0, 11.0);
    glPopMatrix();
    glRotatef(-theta,0,0,1);
    glTranslatef(-x, -y, 0);
}

// Initializing some openGL 3D rendering options
void initRendering() {

	glEnable(GL_DEPTH_TEST);        // Enable objects to be drawn ahead/behind one another
	glEnable(GL_COLOR_MATERIAL);    // Enable coloring
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Setting a background color
}



void renderFunction()
{
    glClearColor(0.937, 0.780, 0.619,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
   // glColor3f(0.937, 0.780, 0.619);
    glBegin(GL_POLYGON);
    	glVertex3f(-30*h/70, 30*h/70, 0.0);
    	glVertex3f(30*h/70, 30*h/70, 0.0);
    	glVertex3f(30*h/70, -30*h/70, 0.0);
    	glVertex3f(-30*h/70, -30*h/70, 0.0);
    glEnd();
    glColor3f(0.97, 0.75, 0.51);
    glPushMatrix();
    glBegin(GL_POLYGON);
    	glVertex3f(-26*h/70, 25*h/70, 0.0);
    	glVertex3f(-25*h/70, 26*h/70, 0.0);
    	glVertex3f(25*h/70, 26*h/70, 0.0);
    	glVertex3f(26*h/70, 25*h/70, 0.0);
    	glVertex3f(26*h/70, -25*h/70, 0.0);
    	glVertex3f(25*h/70, -26*h/70, 0.0);
    	glVertex3f(-25*h/70, -26*h/70, 0.0);
    	glVertex3f(-26*h/70, -25*h/70, 0.0);
    glEnd();
    glColor3f(0.5,0.5,0.5);
    glPushMatrix();
    drawCircle(25*h/70, 25*h/70, 15);
    drawCircle(25*h/70, -25*h/70, 15);
    drawCircle(-25*h/70, -25*h/70, 15);
    drawCircle(-25*h/70, 25*h/70, 15);
    glPopMatrix();
    glColor3f(0.8, 0.498, 0.196);
    glPushMatrix();
    drawCircle(0, 0, 80);
    glPopMatrix();
    //glColor3f(0.71, 0.03, 0.03);
    glColor3f(0.3,0.6,0.2);
    glPushMatrix();
    drawCircle(-4*w/9+25*0,-50,10);
    drawCircle(-4*w/9+25*2,-50,10);
    drawCircle(-4*w/9+25*4,-50,10);
    drawCircle(-4*w/9+25*6,-50,10);
    drawCircle(-4*w/9+25*8,-50,10);
    drawCircle(-4*w/9+25*10,-50,10);
    drawCircle(-4*w/9+25*1,-50+10,10);
    drawCircle(-4*w/9+25*3,-50+10,10);
    drawCircle(-4*w/9+25*5,-50+10,10);
    drawCircle(-4*w/9+25*7,-50+10,10);
    drawCircle(-4*w/9+25*9,-50+10,10);
    glPopMatrix();
    glPushMatrix();
    drawFooter(0, 2*h/7, 0);
    drawFooter(0, -2*h/7, 0);
    drawFooter(2*h/7, 0, 90);
    drawFooter(-2*h/7, 0, 90);
    glPopMatrix();
    glPushMatrix();
    setChips(1);
    int now = time(0)-begin;
    if(mark != now && start == 0){
        score -= (now - mark);
        mark = now;
    }
    vector<string> text;
    char str[100];
    text.push_back("Stats");
    sprintf(str,"  White coins backed      : %d",white);
    text.push_back(str);
    sprintf(str,"  Black coins backed       : %d",black);
    text.push_back(str);
    sprintf(str,"  Red coins                      : %d",red);
    text.push_back(str);
    sprintf(str,"  Strikers                         : %d",strike);
    text.push_back(str); 
    sprintf(str,"  Score                    : %d",score);
    text.push_back(str);
    text.push_back("");
    text.push_back("");
    text.push_back("");
    sprintf(str,"Power : %d",boost);
    text.push_back(str);   
    for(int i=0;i<10;i++){
        char*c;
        glRasterPos3f(400, 40-30*i,0);
        for (c=&text[i][0]; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
        }
    }
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    if(start==-1)
       instructions();
    if(start==1)
        GameOver();
    glutSwapBuffers();
}


void reshape(int w, int h)
{
glViewport (0, 0, (GLsizei) w, (GLsizei) h);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho((GLdouble) -w/2, (GLdouble) w/2, (GLdouble) -h/2, (GLdouble) h/2, -1000.0, 1000.0);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
}

void coinCollision(int p, int q){
    float slope = atan((coin[q].y_pos - coin[p].y_pos)/( coin[q].x_pos - coin[p].x_pos));
    slope = (slope/M_PI*180) + (slope > 0 ? 0 : 360);
    slope = slope*M_PI/180;
    while(distance(coin[q].x_pos,coin[q].y_pos,coin[p].x_pos,coin[p].y_pos)<25){
        float initial = distance(coin[q].x_pos,coin[q].y_pos,coin[p].x_pos,coin[p].y_pos);
        coin[p].x_pos += cos(slope);
        coin[p].y_pos += sin(slope);
        float final = initial - distance(coin[q].x_pos,coin[q].y_pos,coin[p].x_pos,coin[p].y_pos);
        if(final>0){
        coin[p].x_pos -= 2*cos(slope);
        coin[p].y_pos -= 2*sin(slope);
        }
    }
    float v1_x = coin[p].x_vel*cos(slope)+coin[p].y_vel*sin(slope);
    float v2_x = coin[q].x_vel*cos(slope)+coin[q].y_vel*sin(slope);
    float v1_y = coin[p].y_vel;
    float v2_y = coin[q].y_vel;
    coin[p].x_vel = -2*v1_x + v2_x - 0.9*1*(v1_x - v2_x)*cos(slope);
    coin[p].x_vel/=3;
    coin[q].x_vel = 2*v1_x + v2_x + 0.9*1*(v1_x - v2_x)*cos(slope);
    coin[q].x_vel/=3;
    coin[p].y_vel =2*v1_y + v2_y - 0.9*1*(v1_y - v2_y)*sin(slope);
    coin[p].y_vel/=3;
    coin[q].y_vel =2*v1_y + v2_y + 0.9*1*(v1_y - v2_y)*sin(slope);
    coin[q].y_vel/=3;
    coin[q].set();
    coin[p].set();
}

void strikerCollision(int p){
    float slope = atan((coin[p].y_pos - striker.y_pos)/( coin[p].x_pos - striker.x_pos));
    slope = (slope/M_PI*180) + (slope > 0 ? 0 : 360);
    slope = slope*M_PI/180;
    while(distance(striker.x_pos,striker.y_pos,coin[p].x_pos,coin[p].y_pos)<28){
        float initial = distance(striker.x_pos,striker.y_pos,coin[p].x_pos,coin[p].y_pos);
        striker.x_pos += cos(slope);
        striker.y_pos += sin(slope);
        float final = distance(striker.x_pos,striker.y_pos,coin[p].x_pos,coin[p].y_pos);
        if(initial>=final){
            striker.x_pos -= 2*cos(slope);
            striker.y_pos -= 2*sin(slope);
        }
    }
    float v1_x = striker.x_vel*cos(slope)+striker.y_vel*sin(slope);
    float v2_x = coin[p].x_vel*cos(slope)+coin[p].y_vel*sin(slope);
    float v1_y = striker.y_vel;
    float v2_y = coin[p].y_vel;
    striker.x_vel = -2*v1_x + v2_x - 0.9*1*(v1_x - v2_x)*cos(slope);
    striker.x_vel/=3;
    coin[p].x_vel = 2*v1_x + v2_x + 0.9*1*(v1_x - v2_x)*cos(slope);
    coin[p].x_vel/=3;
    striker.y_vel =2*v1_y + v2_y - 0.9*1*(v1_y - v2_y)*sin(slope);
    striker.y_vel/=3;
    coin[p].y_vel =2*v1_y + v2_y + 0.9*1*(v1_y - v2_y)*sin(slope);
    coin[p].y_vel/=3;
    coin[p].set();
    striker.set();
}

void initChips(){
    striker.init(10, -2*h/7, 0, 0, -1);
    coin[10].init(0, 0, 0, 0, 50);
    for(int i=0;i<10;i++){
        coin[i].init(80*cos(2*M_PI*i/10.0),80*sin(2*M_PI*i/10.0),0,0,5*(i%2+1)*pow(-1,i%2+1));
    }
}

void handleKeypress1(unsigned char key, int x, int y) {

    if (key == 27) {
        exit(0);
    }

    if(key==13){
        if(start==-1){
            begin = time(0);
            score = 30;
            mark=0;
            start=0;
        }
        start *= -1;
    }

    if (key == 97 )
        proj_theta=(proj_theta+1)%360;

    if (key == 99){
        if(proj_theta ==0 )
            proj_theta = 360;
        proj_theta=proj_theta-1;
    }

    if(key == 32){
        if(striker.x_vel == 0 && striker.y_vel ==0){
            double theta = proj_theta*M_PI/180;
            striker.x_vel = boost * cos(theta);
            striker.y_vel = boost * sin(theta);
        }
    }
}

float distance(float p1_x, float p1_y, float p2_x, float p2_y){
    double x = p1_x - p2_x;
    x*=x;
    double y = p1_y - p2_y;
    y*=y;
    return sqrt(x+y);
}

void checkScore(){
    if(distance(25*h/70,25*h/70,fabs(striker.x_pos),fabs(striker.y_pos))<18){
        strike++;
        score++;
        striker.x_vel=striker.y_vel=0;
    }
    for (int i = 0; i < 11; i++){
        if(distance(25*h/70,25*h/70,fabs(coin[i].x_pos),fabs(coin[i].y_pos))<20){
            score += coin[i].val;
            coin[i].x_pos = -4*w/9+25*i;
            coin[i].y_pos = -50+10*(i%2);
            coin[i].x_vel=coin[i].y_vel=0;
            if(coin[i].val == -5)
                black++;
            else if(coin[i].val == 50)
                red++;
            else{
                white++;
                if(white == 5)
                    start=1;
            }
        }
    }
}

void setChips(int value){
    striker.set();
    for (int i = 0; i < 11; i++){
        coin[i].set();
        if(distance(striker.x_pos, striker.y_pos,coin[i].x_pos, coin[i].y_pos) < 28){
            strikerCollision(i);
        }
    }
    for (int i = 0; i < 11; i++){
        for (int j = 0; j <11 ; j++){
            if(i!=j && distance(coin[j].x_pos, coin[j].y_pos,coin[i].x_pos, coin[i].y_pos) < 25 ){
            coinCollision(j, i);
            }
        }
    }
    checkScore();
}

void handleKeypress2(int key, int x, int y) {
    if(key == GLUT_KEY_UP && boost < 20)
        boost = (boost+1)%21;
    if(key == GLUT_KEY_DOWN && boost > 1)
        boost = abs(boost - 1) % 21;
    if (key == GLUT_KEY_LEFT && striker.x_pos > -186 && striker.x_vel == 0 && striker.y_vel ==0)
        striker.x_pos = striker.x_pos-1;
    if (key == GLUT_KEY_RIGHT && striker.x_pos < 186 && striker.x_vel == 0 && striker.y_vel ==0)
        striker.x_pos = striker.x_pos+1;
}

void handleMouse(int button, int state, int x, int y){
    x = x - w/2;
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        handleKeypress1(32,0,0);
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && striker.x_vel ==0 && striker.y_vel ==0){
        if(x > 182)
            striker.x_pos=186;
        else if(x<-182)
            striker.x_pos=-186;
        else
            striker.x_pos= x;
    }
}

void passive(int x,int y) {  
    x = x - striker.x_pos - w/2;
    y = y + striker.y_pos - h/2;   //     Inverted mouse
    if(x!=0){
        float slope = atan2(y,x);
        slope = (slope/M_PI*180) + (slope > 0 ? 0 : 360);
        proj_theta = slope;
    }
    else
        proj_theta = 90;
    float p = distance(0,0,x,y)/4;
    if(p<21){
        boost = p;
    }
    else{
        boost=20;
    }
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    w = glutGet(GLUT_SCREEN_WIDTH);
    h = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowSize(w, h);
    glutCreateWindow("Carroms");
    initRendering();
    initChips();
    glutDisplayFunc(renderFunction);
    glutIdleFunc(renderFunction);
    glutPassiveMotionFunc(passive);
	glutReshapeFunc(reshape);
    glutMouseFunc(handleMouse);
    glutKeyboardFunc(handleKeypress1);
    glutSpecialFunc(handleKeypress2);
    glutMainLoop();    
    return 0;
}