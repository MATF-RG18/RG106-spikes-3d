#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>

/* Definisemo granice parametara povrsi */
#define U_FROM -50
#define V_FROM -50
#define U_TO 50
#define V_TO 50

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

static void keys_press(bool k[]);

/* Deklaracija funckija*/
void plot_function();
void set_vertex_and_normal(float u, float v);
void r2_function();

bool keys[256];                 /* Niz za multiple keypress */
static float x_axis, z_axis;    /* parametri za pomeranje igraca po tabli*/
static float t;                 /* Proteklo vreme */
static int animation_ongoing;   /* flag za animaciju (jos uvek ne iskoriscen)*/

int main(int argc, char **argv)
{
    /* Inicijalizacija promenljivih */
    t = 0;

    /* Inicijalizacija Glut-a */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Kreira se prozor */
    glutInitWindowSize(550, 550);
    glutInitWindowPosition(700, 200);
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

  if(k[GLUT_KEY_UP])
    z_axis -= 0.1;
  if(k[GLUT_KEY_DOWN])
    z_axis += 0.1;
  if(k[GLUT_KEY_LEFT])
    x_axis -= 0.1;
  if(k[GLUT_KEY_RIGHT])
    x_axis += 0.1;

  glutPostRedisplay();
}

static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
        /* Zavrsava se program. */
        exit(0);
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
    gluLookAt(0, 10, 10,
              0, 0, 0,
              0, 1, 0);

}

static void on_timer(int value)
{
    /* Proverava se da li callback dolazi od odgovarajuceg tajmera */
    if (value != TIMER_ID) return;
    t += 3;

    /* Forsira se ponovno iscrtavanje prozora */
    glutPostRedisplay();

    if (animation_ongoing) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
}

static void on_display(void)
{
    /* Pozicija svetla */
    GLfloat light_position[] = { 1, 1, 1, 0 };

    /* Boje svetla */
    GLfloat light_ambient[] = { 0.3, 0.3, 0.5, 1 };
    GLfloat light_diffuse[] = { 0.2, 0.3, 0.5, 1 };
    GLfloat light_specular[] = { 0.6, 0.9, 0.6, 1 };

    /* Refleksije materijala */
    GLfloat ambient_coeffs[] = { 0.1, 0.3, 0.6, 1 };
    GLfloat diffuse_coeffs[] = { 0.1, 0.1, 1, 1 };
    GLfloat specular_coeffs[] = { 0.2, 0.1, 1, 1 };

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
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glShadeModel(GL_SMOOTH);

    glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);

    /* Kreiraju se objekati */
    plot_function();
    r2_function();

    /* Funkcija za kretanje */
    keys_press(keys);

    /* Nova slika se salje na ekran */
    glutSwapBuffers();
}

void r2_function(){
  /*
  TODO: -doraditi figuru sa kojom se krece
        -dodati teksture
  */
  GLfloat r2_ambient[] = { 0.8, 0.1, 0.1, 1 };
  GLfloat r2_diffuse[] = { 0.5, 0.5, 0.5, 1 };
  GLfloat r2_specular[] = { 0.6, 0.4, 0.4, 1 };

    glPushMatrix();
      glTranslatef(x_axis, 0.3, z_axis);
      glMaterialfv(GL_FRONT, GL_AMBIENT, r2_ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, r2_diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, r2_specular);
      glutSolidSphere(0.3, 40, 40);
    glPopMatrix();
}

void set_vertex_and_normal(float u, float v)
{
    /*
    Parametar koji odredjuje saru na tabli(temu)
    TODO: vise razlicitih parametara koji ce omogucavati na pocetku igrice da se izabere tema
    */
    float param = (u*v/30+t+(v*v/20)) / 100.0;
    glNormal3f(sin(param), 1, cos(param));

    /* Postavljamo verteks */
    glVertex3f(u, 0, v);
}

/* Crtamo funkciju (kod preuzet sa casa)*/
void plot_function()
{
  float scale;
  int u, v;

  glPushMatrix();

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
