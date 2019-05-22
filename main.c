#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

/* Definisemo granice parametara povrsi */
#define U_FROM -50
#define V_FROM -50
#define U_TO 50
#define V_TO 50
/* Definisemo granice table*/
#define X_WALL 4.5
#define Z_WALL 4.7
/* Definisemo osobine tajmera */
#define TIMER_INTERVAL 10
#define TIMER_ID 0

/* Makro za funkciju max */
#define max(A, B) (((A) > (B)) ? (A) : (B))

/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void arrow_keys(int key, int x, int y);
void arrow_keys_up(int key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);
static void on_timer(int);

/* Deklaracija funckija*/
void plot_function();
void set_vertex_and_normal(float u, float v);
void r2_function();
void coins();
void generate_coin();
void generate_spikes();
void score();
void crash_report();
void Spikes();
void instructions();

void Scoreboard(char *string);
char buffer[10];
char string[] = {"Score:\0"};

static void keys_press(bool k[]);
static int c, k, Sc;
bool keys[256];                 /* Niz za multiple keypress */
static float x_axis, z_axis;    /* Parametri za pomeranje igraca po tabli*/
static float x_coin, z_coin;     /* Koordinate poena*/
static float left_spike[2], right_spike[2];   /* Koorinate siljaka*/
static float top_spike[2], bottom_spike[2];
static int t = 0;                 /* Proteklo vreme */
static float direction;           /* parametar oravca u kome se krecu siljci*/
static int animation_ongoing;   /* flag za animaciju */
static int start = 0;

int main(int argc, char **argv)
{
    /* Inicijalizacija promenljivih */
    srand(time(NULL));
    animation_ongoing = 0;
    c = 0;
    k = 0;
    Sc = 0;
    direction = 0;
    generate_spikes();
    /* Inicijalizacija Glut-a */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Kreira se prozor */
    glutInitWindowSize(900, 700);
    glutInitWindowPosition(550, 200);
    glutCreateWindow("Spikes 3D");

    /* Registruju se callback funkcije */
    glutKeyboardFunc(on_keyboard);
    glutSpecialFunc(arrow_keys);
    glutSpecialUpFunc(arrow_keys_up);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);



    /* Obavlja se OpenGL inicijalizacija */
    glClearColor(0.1, 0.1, 0.1, 0.1);
    glEnable(GL_DEPTH_TEST);

    /* Ukljucujemo normalizaciju vektora normala */
    glEnable(GL_NORMALIZE);

    /* Program ulazi u glavnu petlju. */
    glutMainLoop();

    return 0;
}
/* Funkcije koje omogucavaju kretanje po dijagonali, uz pomoc niza koji proverava da l se drzi jedna ili dve strelice istovremeno*/
static void arrow_keys(int key, int x, int y){
  keys[key] = true;
}
void arrow_keys_up(int key, int x, int y){
	keys[key] = false;
}
static void keys_press(bool k[]){

  if(k[GLUT_KEY_UP]){
    if(z_axis <= -Z_WALL);
    else
      z_axis -= 0.125;    //brzina kretanja po tabli
  }
  if(k[GLUT_KEY_DOWN]){
    if(z_axis >= Z_WALL);
    else
      z_axis += 0.125;
  }
  if(k[GLUT_KEY_LEFT]){
    if(x_axis < -X_WALL - 0.3);
    else
      x_axis -= 0.125;
  }
  if(k[GLUT_KEY_RIGHT]){
    if(x_axis > X_WALL + 0.3);
    else
      x_axis += 0.125;
  }

  glutPostRedisplay();
}

static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
        /* Izlaz iz aplikacije */
        exit(0);
        break;

    case 's':
    case 'S':
        /*
          Pokrece se animacija.
          Nakon resetovanja skora i celokupne aplikacije potrebno je ponovo
           pritisnuti 's' za Start
        */

        if(!start){
          if (!animation_ongoing) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
          }
          start = 1;
        }
        break;
        //Svi parametri se resetuju
    case 'r':
    case 'R':
      direction = 0;
      t = 0;
      Sc = 0;
      c = 0;
      k = 0;
      animation_ongoing = 0;
      start = 0;
      //Ponovno ispisivanje pocetnog skora pri resetu
      sprintf(buffer, "Score: %d", Sc);
      Scoreboard(buffer);
        break;
    }
}

static void on_reshape(int width, int height)
{
    /* Podesava se viewport */
    glViewport(0, 0, width, height);

    /* Podesava se projekcija */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 100);

    /* Podesava se vidna tacka */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 11, 8,
              0, 0, 0,
              0, 1, 0);

}

static void on_timer(int value)
{
    /* Proverava se da li callback dolazi od odgovarajuceg tajmera */
    if (value != TIMER_ID) return;

      t += 1;
      /* Tezina igre*/
      /* Na svakih 100 poena ubrzavaju se siljci*/
      if(Sc < 100)
        direction += 0.07;
      else if(Sc >= 100 && Sc < 200)
        direction += 0.1;
      else if(Sc >= 200 && Sc < 300)
        direction += 0.125;
      else if(Sc >= 300 && Sc < 500)
        direction += 0.15;
      else if(Sc >= 500 && Sc < 600)
        direction += 0.175;
      else
        direction += 0.2;

    /* Forsira se ponovno iscrtavanje prozora */
    glutPostRedisplay();

    if (animation_ongoing) {
         glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
     }
}

static void on_display(void)
{
    /* Pozicija svetla */
    GLfloat light_position[] = { 0, 1, -1, 0 };

    /* Boje svetla */
    GLfloat light_ambient[] = { 0.3, 0.3, 0.5, 1 };
    GLfloat light_diffuse[] = { 0.2, 0.3, 0.5, 1 };
    GLfloat light_specular[] = { 0.6, 0.9, 0.6, 1 };

    /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Ukljucuje se osvjetljenje i podesavaju parametri svetla */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glShadeModel(GL_SMOOTH);

    /* Kreiraju se objekati */
    Scoreboard(buffer);
    if(!start){
      instructions();
    }
    plot_function();
    r2_function();

    if(animation_ongoing){
      score();
      coins();
    }
    Spikes();

    /* Funkcija za kretanje */
    keys_press(keys);

    if(start && (!animation_ongoing))
      instructions();

    crash_report();
    /* Nova slika se salje na ekran */
    glutSwapBuffers();
}
void crash_report(){

  if((x_axis>left_spike[0]-0.1 && x_axis<left_spike[0]+1) && (z_axis>left_spike[1]-0.35 && z_axis<left_spike[1]+0.1))
    animation_ongoing = 0;
  if((x_axis>right_spike[0]-1 && x_axis<right_spike[0]+0.1) && (z_axis>right_spike[1]-0.35 && z_axis<right_spike[1]+0.25))
    animation_ongoing = 0;
  if((x_axis>top_spike[0]-0.25 && x_axis<top_spike[0]+0.25) && (z_axis>top_spike[1]-0.1 && z_axis<top_spike[1]+1))
    animation_ongoing = 0;
  if((x_axis>bottom_spike[0]-0.25 && x_axis<bottom_spike[0]+0.25) && (z_axis>bottom_spike[1]-1.4 && z_axis<bottom_spike[1]-0.1))
    animation_ongoing = 0;

}
void instructions(){
  char inst[] = {"Press r to reset Score\0"};
  char init_inst[] = {"Press s to start\0"};
  char end[] = {"GAME OVER\0"};
  int len, i;
  GLfloat red_ambient[] = { 1, 0, 0, 1 };
  GLfloat white_ambient[] = { 1, 1, 1, 1 };
  if(start){
    glPushMatrix();
      glMaterialfv(GL_FRONT, GL_AMBIENT, red_ambient);
      glRasterPos3f(-0.4, 7, 0.3);
      len = (int)strlen(end);
      for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, end[i]);
      }
    glPopMatrix();
    glPushMatrix();
      glMaterialfv(GL_FRONT, GL_AMBIENT, white_ambient);
      glRasterPos3f(-1.1, 7, 1);
      len = (int)strlen(inst);
      for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, inst[i]);
      }
    glPopMatrix();
  }
  else{
    glPushMatrix();
      glMaterialfv(GL_FRONT, GL_AMBIENT, white_ambient);
      glRasterPos3f(-0.8, 7, 1);
      len = (int)strlen(init_inst);
      for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, init_inst[i]);
      }
    glPopMatrix();
  }
}
/* Funkcija koja postavlja x i z koordinate na nasumicnim mestima na tabli */
void generate_coin(){
  x_coin = (double)rand()/RAND_MAX*2.0*X_WALL-X_WALL;
  z_coin = (double)rand()/RAND_MAX*2.0*Z_WALL-Z_WALL;
}
void score(){
/*Pri svakom sakupljenom poenu poziva se f-ja za postavljanje na novom mestu i uvecava se Score*/
  if((k == c)){
    generate_coin();
    Sc = c * 10;
    c++;
    sprintf(buffer, "Score: %d", Sc);
  }
  if((x_axis>x_coin-0.5 && x_axis<x_coin+0.5) && (z_axis>z_coin-0.5 && z_axis<z_coin+0.5))
    k++;

}
/*prosledjuje se string u kome se cuva trenutni rezultat(Sc) i ispisuje se na ekran*/
void Scoreboard(char *string)
{
  int len, i;
  GLfloat s_ambient[] = { 1, 0.9, 0.9, 1 };

  glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glRasterPos3f(-0.33, 7, 2);
    len = (int)strlen(string);
    for (i = 0; i < len; i++) {
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
    }
  glPopMatrix();
  glutPostRedisplay();
}
void Spikes(){
  /* Podesavaju se parametri materijala */
  GLfloat s_ambient[] = { 0.9, 0.4, 0, 1 };
  GLfloat s_diffuse[] = { 0.9, 0.4, 0, 1 };
  GLfloat s_specular[] = { 0.9, 0.4, 0, 1 };
  GLfloat shininess = 40;

  /* Spoljasnji zid*/
  glPushMatrix();
    glTranslatef(-X_WALL - 2, 0.5, -0.3);
    glScalef(0.35, 0.5, Z_WALL * 2 + 3.5);
    glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, s_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, s_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
    glTranslatef(X_WALL + 2, 0.5, -0.3);
    glScalef(0.35, 0.5, Z_WALL * 2 + 3.5);
    glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, s_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, s_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
    glTranslatef(0, 0.5, -Z_WALL - 2);
    glScalef(X_WALL * 3 - 0.15,0.5,0.35);
    glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, s_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, s_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
    glTranslatef(0, 0.5, Z_WALL + 1.5);
    glScalef(X_WALL * 3 - 0.15,0.5,0.35);
    glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, s_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, s_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glutSolidCube(1);
  glPopMatrix();

  /* SILJCI */

  if(direction >= 12)
    generate_spikes();

  if(start){
    /* left_spike*/
    left_spike[0] = -X_WALL - 2 + direction;
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, s_diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, s_specular);
      glMaterialf(GL_FRONT, GL_SHININESS, shininess);

      glTranslatef(left_spike[0], 0.5, left_spike[1]);
      glRotatef(90, 0, 1, 0);
      glutSolidCone(0.2, 0.7, 50, 50);
    glPopMatrix();

    /*right_spike*/
    right_spike[0] = X_WALL + 2 - direction;
    glPushMatrix();
      glTranslatef(right_spike[0], 0.5, right_spike[1]);
      glRotatef(-90, 0, 1, 0);
      glutSolidCone(0.2, 0.7, 50, 50);
    glPopMatrix();

    /*top_spike*/
    top_spike[1] = -Z_WALL - 2 + direction;
    glPushMatrix();
      glTranslatef(top_spike[0], 0.5, top_spike[1]);
      //glRotatef(0, 0, 1, 0);
      glutSolidCone(0.2, 0.7, 50, 50);
    glPopMatrix();

    /*bottom_spike*/
    bottom_spike[1] = Z_WALL + 1.5 - direction;
    glPushMatrix();
      glTranslatef(bottom_spike[0], 0.5, bottom_spike[1]);
      glRotatef(180, 0, 1, 0);
      glutSolidCone(0.2, 0.7, 50, 50);
    glPopMatrix();
  }
}
void generate_spikes(){

  direction = 0;
  left_spike[1] = (double)rand()/RAND_MAX*2.0*Z_WALL-Z_WALL;
  right_spike[1] = (double)rand()/RAND_MAX*2.0*Z_WALL-Z_WALL;
  top_spike[0] = (double)rand()/RAND_MAX*2.0*X_WALL-X_WALL;
  bottom_spike[0] = (double)rand()/RAND_MAX*2.0*X_WALL-X_WALL;

}
void coins(){
  /* Podesavaju se parametri materijala */
  GLfloat c_ambient[] = { 1, 1, 0, 1 };
  GLfloat c_diffuse[] = { 1, 1, 0, 1 };
  GLfloat c_specular[] = { 1, 1, 0, 1 };
  GLfloat shininess = 90;

  glPushMatrix();
    glTranslatef(x_coin, 0.3, z_coin);
    glRotatef(t*4, 0, 1, 0);
    glScalef(1,1,0.3);
    glMaterialfv(GL_FRONT, GL_AMBIENT, c_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, c_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, c_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glutSolidSphere(0.3, 50,50);
  glPopMatrix();


}
void r2_function(){
  /* Podesavaju se parametri materijala */
  GLfloat r2_ambient[] = { 0.2, 0, 0.8, 1 };
  GLfloat r2_diffuse[] = { 0.2, 0, 0.8, 1 };
  GLfloat r2_specular[] = { 0.2, 0, 0.8, 1 };
  GLfloat rc_ambient[] = { 0.9, 0.9, 0.9, 1 };
  GLfloat rc_diffuse[] = { 0.9, 0.9, 0.9, 1 };
  GLfloat rc_specular[] = { 0.9, 0.9, 0.9, 1 };

    glPushMatrix();
      //Telo(kupa ispod lopte)
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, rc_ambient);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rc_diffuse);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, rc_specular);
      glTranslatef(x_axis, 0.65 , z_axis);
      glRotatef(90, 1,0,0);
      glutSolidCone(0.3,0.65,40,40);

      // Glava(lotpa)
      glMaterialfv(GL_FRONT, GL_AMBIENT, r2_ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, r2_diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, r2_specular);
      glTranslatef(0,0,-0.3);
      glutSolidSphere(0.3, 40, 40);
    glPopMatrix();
}

void set_vertex_and_normal(float u, float v)
{
    /*
    Parametar koji odredjuje saru na tabli(temu)
    */
    float param;
    if(Sc < 200)
      param = (u*v/30+(t*3)+(v*v/20)) / 100.0;
    else if(Sc >= 200 && Sc < 300)
      param = (u*u/30+(t*2)-(u*u)) / 100.0;
    else if(Sc >= 300 && Sc < 400)
      param = (u*u/30+(t)+(v*v)) / 50.0;
    else if(Sc >= 400 && Sc < 500)
      param = (v*u-(t*5)+(u*v)) / 100.0;
    else
      param = (v*u-(t*5)+(u*v)) / 10.0;

    glNormal3f(sin(param), 1, cos(param));

    /* Postavljamo verteks */
    glVertex3f(u, 0, v);
}

/* Crtamo funkciju (kod preuzet sa casa)*/
void plot_function()
{
  float scale;
  int u, v;
  /* Podesavaju se parametri materijala */

    GLfloat ambient_coeffs[] = { 0.2, 0.4, 0.1, 1 };
    GLfloat diffuse_coeffs[] = { 0.2, 0.4, 0.1, 1 };
    GLfloat specular_coeffs[] = { 0.2, 0.4, 0.1, 1 };
    GLfloat shininess = 10;

    GLfloat ambient_coeffs_black[] = { 0.01, 0.01, 0.01, 1 };
    GLfloat diffuse_coeffs_black[] = { 0.02, 0.04, 0.01, 1 };
    GLfloat specular_coeffs_black[] = { 0.02, 0.04, 0.01, 1 };


  glPushMatrix();
  if(Sc < 500){
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
  }
  else{
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs_black);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs_black);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs_black);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
  }
  /* Racunamo faktor skaliranja tako da cela funkcija bude prikazana */
  scale =  10.0/max( U_TO - U_FROM, V_TO - V_FROM );
  glScalef(scale, scale, scale);

  /* Crtamo funkciju strip po strip */
  for (u = U_FROM; u < U_TO; u++) {
    glBegin(GL_TRIANGLE_STRIP);
    for (v = V_FROM; v <= V_TO; v++) {
      set_vertex_and_normal(u, v);
      set_vertex_and_normal(u + 1, v);
    }
    glEnd();
  }

  glPopMatrix();
}
