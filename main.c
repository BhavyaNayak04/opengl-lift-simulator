/* lift_simulation.c
 *
 * Simplified, functional OpenGL/GLUT program implementing the "Simulation of a Lift"
 *
 * Controls:
 * - Left mouse button  : open lift at ground floor
 * - Middle mouse button: open lift at first floor
 * - Right mouse button : open lift at second floor
 * - 'e' : enter lift
 * - 'f' : exit at first floor
 * - 's' : exit at second floor
 * - 'E' : exit to ground floor
 */

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

/* simple typedef for readable 3-int points */
typedef int point3[3];

/* vertex positions used by a few base polygons */
GLint vertices[][3] = {
    {40,40,0},{40,50,0},{400,50,0},{400,40,0},
    {90,75,0},{90,65,0},{430,75,0},{430,65,0},
    {0,0,0},{0,70,0},{499,70,0},{499,0,0},
    {137,0,0},{137,5,0},{142,5,0},{142,0,0},
    {175,30,0},{175,32,0},{177,32,0},{177,30,0},
    {272,0,0},{272,5,0},{277,5,0},{277,0,0},
    {237,30,0},{237,32,0},{239,32,0},{239,30,0}
};

/* offsets for floors (ground, first, second) */
int off[3] = {0, 100, 200};

/* interaction flags:
 *  flag  : which mouse button / floor is currently active (0 none, 1 ground, 2 first, 3 second)
 *  flag1 : keyboard action state (0 none, 1 enter, 2 exit at 1st, 3 exit at 2nd, 4 exit to ground)
 */
int flag = 0;
int flag1 = 0;

/* forward declarations */
void scaleman(void);
void man(void);
void liftdoor(int k);
void liftClosed(int k);
void entry(int offset);
void keys(unsigned char key, int x, int y);
void mouse(int btn, int state, int x, int y);
void ground(void);
void building(void);
void grill(void);
void bottom(void);
void polygon_poly(int a, int b, int c, int d);
void drawface(point3 a, point3 b, point3 c, point3 d);

void man(void)
{
    /* draw a stylized person at the canonical location near (270,105) */
    glColor3ub(0,0,0);
    glPushMatrix();
        glTranslatef(270,105,0);
        glutSolidTorus(1,15,100,90); /* hair / head outline */
    glPopMatrix();

    /* face fill */
    glColor3ub(255,191,128);
    glPushMatrix();
        glTranslatef(270,105,0);
        glutSolidTorus(7,7,100,90);
    glPopMatrix();

    /* nose & eyebrow lines */
    glColor3ub(0,0,0);
    glBegin(GL_LINES);
        glVertex2i(270,107); glVertex2i(270,100);
        glVertex2i(261,113); glVertex2i(267,113);
        glVertex2i(273,113); glVertex2i(279,113);
    glEnd();

    /* eyes */
    glBegin(GL_POLYGON);
        glVertex2i(263,111); glVertex2i(265,111); glVertex2i(265,109); glVertex2i(263,109);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2i(275,111); glVertex2i(277,111); glVertex2i(277,109); glVertex2i(275,109);
    glEnd();

    /* mouth */
    glBegin(GL_POLYGON);
        glVertex2i(266,97); glVertex2i(270,95); glVertex2i(274,97); glVertex2i(270,95);
    glEnd();

    /* shirt */
    glBegin(GL_POLYGON);
        glColor3ub(55,50,70);
        glVertex2i(259,90); glVertex2i(281,90); glVertex2i(296,79);
        glVertex2i(291,71); glVertex2i(286,75); glVertex2i(286,55);
        glVertex2i(254,55); glVertex2i(254,75); glVertex2i(249,70);
        glVertex2i(244,79);
    glEnd();

    /* hands (left and right) */
    glBegin(GL_POLYGON);
        glColor3ub(255,191,128);
        glVertex2i(295,78); glVertex2i(305,63); glVertex2i(297,64); glVertex2i(292,72);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2i(305,63); glVertex2i(286,48); glVertex2i(286,55); glVertex2i(297,64);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2i(245,78); glVertex2i(235,63); glVertex2i(243,64); glVertex2i(248,72);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2i(235,63); glVertex2i(254,48); glVertex2i(254,55); glVertex2i(243,64);
    glEnd();

    /* belt */
    glBegin(GL_POLYGON);
        glColor3ub(100,120,130);
        glVertex2i(286,55); glVertex2i(254,55); glVertex2i(254,50); glVertex2i(286,50);
    glEnd();

    /* collar (two pieces) */
    glBegin(GL_POLYGON);
        glColor3ub(200,140,110);
        glVertex2i(259,90); glVertex2i(281,90); glVertex2i(276,80); glVertex2i(264,80);
    glEnd();
    glBegin(GL_TRIANGLES);
        glColor3ub(20,140,110);
        glVertex2i(270,87); glVertex2i(275,80); glVertex2i(265,80);
    glEnd();

    /* buttons */
    glColor3ub(0,0,0);
    glPushMatrix(); glTranslatef(270,75,0); glutSolidTorus(1,1,100,90); glPopMatrix();
    glPushMatrix(); glTranslatef(270,68,0); glutSolidTorus(1,1,100,90); glPopMatrix();
    glPushMatrix(); glTranslatef(270,61,0); glutSolidTorus(1,1,100,90); glPopMatrix();

    /* pants */
    glBegin(GL_POLYGON);
        glColor3ub(10,10,10);
        glVertex2i(285,50); glVertex2i(254,50); glVertex2i(250,15); glVertex2i(260,15);
        glVertex2i(263,48); glVertex2i(280,15); glVertex2i(290,15);
    glEnd();
}

void scaleman(void)
{
    glPushMatrix();
        glTranslated(270,105,0);
        glScaled(0.3,0.6,1.0);
        glTranslated(-270,-105,0);
        man();
    glPopMatrix();
}

/* polygon helper: uses explicit colors to keep things simple */
void polygon_poly(int a, int b, int c, int d)
{
    glBegin(GL_POLYGON);
        /* we'll set color outside as needed before calling this function */
        glVertex3iv(vertices[a]);
        glVertex3iv(vertices[b]);
        glVertex3iv(vertices[c]);
        glVertex3iv(vertices[d]);
    glEnd();
}

void drawface(point3 a, point3 b, point3 c, point3 d)
{
    glBegin(GL_POLYGON);
        glVertex3iv(a); glVertex3iv(b); glVertex3iv(c); glVertex3iv(d);
    glEnd();
}

void liftClosed(int k)
{
    /* closed doors background rectangle (simple) */
    GLint bg[4][3] = {
        {290,55+off[k],0}, {290,130+off[k],0}, {330,130+off[k],0}, {330,55+off[k],0}
    };
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_POLYGON);
        glVertex3iv(bg[0]); glVertex3iv(bg[1]); glVertex3iv(bg[2]); glVertex3iv(bg[3]);
    glEnd();
}

void liftClosedDoorsOverlay(int k)
{
    /* two vertical door panels (closed) */
    GLint d[][3] = {
        {290,55+off[k],0},{290,130+off[k],0},{309,130+off[k],0},{309,55+off[k],0},
        {310,55+off[k],0},{310,130+off[k],0},{330,130+off[k],0},{330,55+off[k],0}
    };

    glColor3f(0.0f,0.25f,0.25f);
    glBegin(GL_POLYGON);
        glVertex3iv(d[0]); glVertex3iv(d[1]); glVertex3iv(d[2]); glVertex3iv(d[3]);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex3iv(d[4]); glVertex3iv(d[5]); glVertex3iv(d[6]); glVertex3iv(d[7]);
    glEnd();
}

void liftdoor(int k)
{
    /* opened door effect: scale each panel horizontally to simulate sliding open */
    GLint d[][3] = {
        {290,55+off[k],0},{290,130+off[k],0},{309,130+off[k],0},{309,55+off[k],0},
        {310,55+off[k],0},{310,130+off[k],0},{330,130+off[k],0},{330,55+off[k],0}
    };

    glColor3f(0.0f,0.25f,0.25f);

    glPushMatrix();
        glTranslatef(290.0f,55.0f,0.0f);
        glScalef(0.2f,1.0f,1.0f);
        glTranslatef(-290.0f,-55.0f,0.0f);
        glBegin(GL_POLYGON); glVertex3iv(d[0]); glVertex3iv(d[1]); glVertex3iv(d[2]); glVertex3iv(d[3]); glEnd();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(330.0f,55.0f,0.0f);
        glScalef(0.2f,1.0f,1.0f);
        glTranslatef(-330.0f,-55.0f,0.0f);
        glBegin(GL_POLYGON); glVertex3iv(d[4]); glVertex3iv(d[5]); glVertex3iv(d[6]); glVertex3iv(d[7]); glEnd();
    glPopMatrix();

    glutPostRedisplay();
}

/* handle man entering / exiting depending on offset and flag1 */
void entry(int offset)
{
    if (offset == 0) { /* ground floor */
        if (flag1 == 1 || flag1 == 2 || flag1 == 3) { /* enter */
            glPushMatrix(); glTranslated(40,5,0); scaleman(); glPopMatrix();
        }
        if (flag1 == 4) { /* exit to ground (move out) */
            glPushMatrix(); glTranslated(-40,-5,0); scaleman(); glPopMatrix();
        }
    } else if (offset == 1) { /* first floor exit */
        glPushMatrix();
            glTranslated(270,205,0);
            glScaled(1.0,0.8,1.0);
            glTranslated(-270,-105,0);
            scaleman();
        glPopMatrix();
    } else { /* second floor exit */
        glPushMatrix();
            glTranslated(270,300,0);
            glScaled(1.0,0.7,1.0);
            glTranslated(-270,-105,0);
            scaleman();
        glPopMatrix();
    }
    glutPostRedisplay();
}

void keys(unsigned char key, int x, int y)
{
    (void)x; (void)y;
    if (key == 'e') flag1 = 1;  /* enter */
    else if (key == 'f') flag1 = 2; /* exit 1st */
    else if (key == 's') flag1 = 3; /* exit 2nd */
    else if (key == 'E') flag1 = 4; /* exit to ground */
    glFlush();
}

void mouse(int btn, int state, int x, int y)
{
    (void)x; (void)y;
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)  { flag = 1; liftdoor(0); }
    if (btn == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN){ flag = 2; liftdoor(1); }
    if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) { flag = 3; liftdoor(2); }
    glFlush();
}

void ground(void)
{
    int i, j, k;
    for (i = 0; i < 3; ++i) {
        GLint cur[][3] = {
            {83,80+off[i],0},{83,100+off[i],0},{83,120+off[i],0},{93,120+off[i],0},{93,80+off[i],0},
            {143,80+off[i],0},{143,100+off[i],0},{133,80+off[i],0},{143,120+off[i],0},{133,120+off[i],0}
        };
        GLint door[][3] = {
            {160,55+off[i],0},{160,140+off[i],0},{220,140+off[i],0},{220,55+off[i],0},
            {170,70+off[i],0},{205,70+off[i],0}
        };
        GLint wind[][3] = {
            {83,80+off[i],0},{83,120+off[i],0},{143,120+off[i],0},{143,80+off[i],0},
            {100,80+off[i],0},{100,120+off[i],0}
        };
        GLint shut[][3] = {
            {160,55+off[i],0},{160,140+off[i],0},{170,125+off[i],0},{170,65+off[i],0},
            {205,65+off[i],0},{205,125+off[i],0},{220,140+off[i],0},{220,55+off[i],0}
        };

        /* door */
        glColor3f(0.79f,0.90f,0.0f);
        drawface(door[0],door[1],door[2],door[3]);
        glColor3f(0.84f,0.84f,0.74f);
        drawface(door[0],door[4],door[5],door[3]);
        glColor3f(0.25f,0.0f,0.0f);
        glBegin(GL_LINE_LOOP);
            glVertex3iv(door[0]); glVertex3iv(door[1]); glVertex3iv(door[2]); glVertex3iv(door[3]);
        glEnd();

        /* door shutters */
        glBegin(GL_POLYGON); glVertex3iv(shut[0]); glVertex3iv(shut[1]); glVertex3iv(shut[2]); glVertex3iv(shut[3]); glEnd();
        glBegin(GL_POLYGON); glVertex3iv(shut[4]); glVertex3iv(shut[5]); glVertex3iv(shut[6]); glVertex3iv(shut[7]); glEnd();

        /* windows */
        glColor3f(0.79f,0.90f,0.0f);
        drawface(wind[4],wind[5],wind[2],wind[3]);
        drawface(wind[0],wind[1],wind[5],wind[4]);
        glColor3f(0.82f,0.6f,1.0f);

        /* curtains (polygons) */
        glBegin(GL_POLYGON); glVertex3iv(cur[1]); glVertex3iv(cur[2]); glVertex3iv(cur[3]); glEnd();
        glBegin(GL_POLYGON); glVertex3iv(cur[0]); glVertex3iv(cur[1]); glVertex3iv(cur[4]); glEnd();
        glBegin(GL_POLYGON); glVertex3iv(cur[5]); glVertex3iv(cur[6]); glVertex3iv(cur[7]); glEnd();
        glBegin(GL_POLYGON); glVertex3iv(cur[6]); glVertex3iv(cur[8]); glVertex3iv(cur[9]); glEnd();

        for (k = 0; k < 3; ++k) {
            int s[] = {0,20,40};
            for (j = 0; j < 3; ++j) {
                glColor3f(0.25f,0.0f,0.0f);
                glPointSize(5.0f);
                glBegin(GL_LINE_LOOP);
                    glVertex3i(83+s[j],80+off[k],0);
                    glVertex3i(83+s[j],120+off[k],0);
                    glVertex3i(103+s[j],120+off[k],0);
                    glVertex3i(103+s[j],80+off[k],0);
                glEnd();
            }
        }
    }
}

void building(void)
{
    int k;
    for (k = 0; k < 3; ++k) {
        GLint w[][3] = {
            {75,55+off[k],0},{75,155+off[k],0},{375,155+off[k],0},{375,55+off[k],0},
            {100,70+off[k],0},{100,170+off[k],0},{400,170+off[k],0},{400,70+off[k],0}
        };
        GLint c[][3] = {
            {50,154+off[k],0},{50,170+off[k],0},{375,170+off[k],0},{375,154+off[k],0},
            {85,175+off[k],0},{85,190+off[k],0},{410,190+off[k],0},{410,175+off[k],0}
        };
        GLint l[][3] = { {285,55+off[k],0},{335,55+off[k],0},{335,155+off[k],0},{285,155+off[k],0} };
        GLint bg[][3] = {
            {290,55+off[k],0},{290,130+off[k],0},{330,130+off[k],0},{330,55+off[k],0},
            {300,65+off[k],0},{300,130+off[k],0},{320,130+off[k],0},{320,65+off[k],0}
        };

        glColor3f(0.0f,0.5f,0.5f);
        drawface(w[0],w[4],w[7],w[3]);
        drawface(w[5],w[4],w[7],w[6]);
        drawface(w[1],w[5],w[4],w[0]);
        glColor3f(0.0f,0.4f,0.4f);
        drawface(w[2],w[6],w[7],w[3]);
        glColor3f(0.0f,0.5f,0.5f);
        drawface(w[0],w[1],w[2],w[3]);
        glColor3f(0.6f,0.6f,0.6f);
        drawface(w[1],w[5],w[6],w[2]);

        drawface(c[0],c[4],c[7],c[3]);
        glColor3f(0.29f,0.29f,0.0f);
        drawface(c[5],c[4],c[7],c[6]);
        drawface(c[1],c[5],c[4],c[0]);

        glColor3f(0.5f,0.0f,0.5f); drawface(l[0],l[1],l[2],l[3]);

        glColor3f(0.3f,0.3f,0.3f); drawface(bg[0],bg[1],bg[5],bg[4]);
        glColor3f(0.42f,0.42f,0.42f); drawface(bg[5],bg[4],bg[7],bg[6]);
        glColor3f(0.5f,0.5f,0.5f); drawface(bg[6],bg[7],bg[3],bg[2]);
        glColor3f(0.6f,0.6f,0.6f); drawface(bg[0],bg[4],bg[7],bg[3]);
    }
    ground();
}

void grill(void)
{
    int i;
    for (i = 0; i < 3; ++i) {
        GLint c[][3] = {
            {50,154+off[i],0},{50,170+off[i],0},{375,170+off[i],0},{375,154+off[i],0},
            {85,175+off[i],0},{85,190+off[i],0},{410,190+off[i],0},{410,175+off[i],0}
        };
        glColor3f(0.5f,0.5f,0.0f); drawface(c[0],c[1],c[2],c[3]);
        glColor3f(0.3f,0.3f,0.0f); drawface(c[2],c[6],c[7],c[3]);
    }
}

void bottom(void)
{
    /* use polygon_poly with vertex indices to draw base shapes */
    glColor3f(0.4f,0.6f,0.4f);
    polygon_poly(0,5,7,3);
    polygon_poly(4,5,7,6);
    polygon_poly(0,1,4,5);
    polygon_poly(2,3,7,6);
    polygon_poly(0,1,2,3);
    polygon_poly(1,4,6,2);
    polygon_poly(12,13,14,15);
    polygon_poly(16,17,18,19);
    polygon_poly(15,14,18,19);
    polygon_poly(13,14,18,17);
    polygon_poly(20,21,22,23);
    polygon_poly(24,25,26,27);
    polygon_poly(20,21,25,24);
    polygon_poly(21,22,26,25);
}

void init(void)
{
    glClearColor(0.0f,1.0f,1.0f,1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,499.0,0.0,499.0);
}

void roadStrips(int r)
{
    int roff[] = {0,15};
    float rstrip[4][3] = {
        {204.5f,5.0f+roff[r],0.0f},
        {209.5f,5.0f+roff[r],0.0f},
        {208.0f,15.0f+roff[r],0.0f},
        {206.0f,15.0f+roff[r],0.0f}
    };
    glColor3f(1.0f,1.0f,1.0f);
    glBegin(GL_POLYGON);
        glVertex3fv(rstrip[0]); glVertex3fv(rstrip[1]); glVertex3fv(rstrip[2]); glVertex3fv(rstrip[3]);
    glEnd();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    /* lawn */
    glColor3f(0.0f,0.5f,0.0f);
    polygon_poly(8,9,10,11);

    /* some black roof base */
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_POLYGON);
        glVertex3f(15.0f,30.0f,0.0f); glVertex3f(75.0f,65.0f,0.0f);
        glVertex3f(455.0f,65.0f,0.0f); glVertex3f(420.0f,30.0f,0.0f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex3f(142.0f,0.0f,0.0f); glVertex3f(177.0f,30.0f,0.0f);
        glVertex3f(237.0f,30.0f,0.0f); glVertex3f(272.0f,0.0f,0.0f);
    glEnd();

    roadStrips(0); roadStrips(1);
    bottom();
    building();

    /* behaviour depending on which mouse button (flag) is active */
    if (flag == 1) { /* ground floor opened */
        liftdoor(0);
        if (flag1 == 1) { entry(0); }
        else if (flag1 == 2 || flag1 == 3) entry(0);
        else if (flag1 == 4) { entry(0); liftClosed(0); }
        else { glPushMatrix(); glTranslated(270,105,0); scaleman(); glPopMatrix(); }
        liftClosed(1); liftClosed(2);
    } else if (flag == 2) { /* first floor opened */
        liftdoor(1);
        if (flag1 == 2) { entry(1); liftClosed(1); }
        else {
            glPushMatrix(); glTranslated(310,205,0); glScaled(1.0,0.8,1.0); glTranslated(-270,-105,0); scaleman(); glPopMatrix();
        }
        liftClosed(0); liftClosed(2);
    } else if (flag == 3) { /* second floor opened */
        liftdoor(2);
        if (flag1 == 3) { entry(2); liftClosed(2); }
        else {
            glPushMatrix(); glTranslated(310,300,0); glScaled(1.0,0.7,1.0); glTranslated(-270,-105,0); scaleman(); glPopMatrix();
        }
        liftClosed(0); liftClosed(1);
    } else { /* no mouse interaction, show closed doors on all floors */
        liftClosed(0); liftClosed(1); liftClosed(2);
    }

    if (flag1 == 0) scaleman(); /* when no keyboard interaction show the person in default place */

    grill();
    glFlush();
}

int main(int argc, char **argv)
{
    printf("Click left mouse button to open the lift (ground floor)\n");
    printf("Press 'e' to enter the lift\n");
    printf("Click middle mouse button to go to the first floor\n");
    printf("Press 'f' to exit at first floor\n");
    printf("Click right mouse button to go to the second floor\n");
    printf("Press 's' to exit at second floor\n");
    printf("Press 'E' to exit at ground floor\n");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(50,50);
    glutInitWindowSize(500,500);
    glutCreateWindow("THE ELEVATOR - Lift Simulation");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keys);
    glutMainLoop();
    return 0;
}
