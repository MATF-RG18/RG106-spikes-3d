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
static float direction;
static int animation_ongoing;   /* flag za animaciju */
static bool crash;

int main(int argc, char **argv)
{
    /* Inicijalizacija promenljivih */
    srand(time(NULL));
      //animation_ongoing = 1;
    c = 0;
    k = 0;
    Sc = 0;
    direction = 0;
    crash = false;
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
      z_axis -= 0.1;
  }
  if(k[GLUT_KEY_DOWN]){
    if(z_axis >= Z_WALL);
    else
      z_axis += 0.1;
  }
  if(k[GLUT_KEY_LEFT]){
    if(x_axis < -X_WALL);
    else
      x_axis -= 0.1;
  }
  if(k[GLUT_KEY_RIGHT]){
    if(x_axis > X_WALL);
    else
      x_axis += 0.1;
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

    case 'g':
    case 'G':
        /* Pokrece se animacija. */
        if (!animation_ongoing) {
            glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            animation_ongoing = 1;
        }
        break;

    case 'r':
    case 'R':
        /* Resetuje se proteklo vreme */
        t = 0;
        glutPostRedisplay();
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
      direction += 0.1;

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

    /* Refleksije materijala */


    /* Koeficijent glatkosti materijala */
    GLfloat shininess = 30;

    /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Ukljucuje se osvjetljenje i podesavaju parametri svetla */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    /* Podesavaju se parametri materijala */


    glShadeModel(GL_SMOOTH);

    glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);


    /* Kreiraju se objekati */
    //crash_report();      TODO
    plot_function();
    r2_function();

    coins();
    Spikes();

    /* Funkcija za kretanje */
    keys_press(keys);

    score();
    Scoreboard(buffer);

    /* Nova slika se salje na ekran */
    glutSwapBuffers();
}
void crash_report(){

  if((x_axis>left_spike[0]-0.5 && x_axis<left_spike[0]+0.5))
    animation_ongoing = 0;

}
/* Funkcija koja postavlja x i z koordinate na nasumicnim mestima na tabli */
void generate_coin(){
  x_coin = (double)rand()/RAND_MAX*2.0*X_WALL-X_WALL;
  z_coin = (double)rand()/RAND_MAX*2.0*Z_WALL-Z_WALL;
}
void score(){
/*Pri svakom sakupljenom poenu poziva se f-ja za postavljanje na novom mestu i uvecava se Score*/
  if(k == c){
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
}
void Spikes(){

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
  if(t%120 == 0)
    generate_spikes();

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
    //glRotatef(90, 0, 1, 0);
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
void generate_spikes(){

  direction = 0;
  left_spike[1] = (double)rand()/RAND_MAX*2.0*Z_WALL-Z_WALL;
  right_spike[1] = (double)rand()/RAND_MAX*2.0*Z_WALL-Z_WALL;
  top_spike[0] = (double)rand()/RAND_MAX*2.0*X_WALL-X_WALL;
  bottom_spike[0] = (double)rand()/RAND_MAX*2.0*X_WALL-X_WALL;

}
void coins(){

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
    TODO: vise razlicitih parametara koji ce se menjati sa porastom poena
    */
    float param = (u*v/30+(t*3)+(v*v/20)) / 100.0;
    glNormal3f(sin(param), 1, cos(param));

    /* Postavljamo verteks */
    glVertex3f(u, 0, v);
}

/* Crtamo funkciju (kod preuzet sa casa)*/
void plot_function()
{
  float scale;
  int u, v;

  GLfloat ambient_coeffs[] = { 0.2, 0.4, 0.1, 1 };
  GLfloat diffuse_coeffs[] = { 0.2, 0.4, 0.1, 1 };
  GLfloat specular_coeffs[] = { 0.2, 0.4, 0.1, 1 };
  GLfloat shininess = 10;

  glPushMatrix();

  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);
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
