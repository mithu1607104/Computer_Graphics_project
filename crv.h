#ifndef CRV_H_INCLUDED
#define CRV_H_INCLUDED

void scsToWcs(float sx,float sy, float wcsv[3] );
void processMouse(int button, int state, int x, int y);
void matColor(float kdr, float kdg, float kdb,  float shiny, int frnt_Back, float ambFactor, float specFactor);
long long nCr(int n, int r);
void BezierCurve ( double t,  float xy[2]);
void setNormal(GLfloat x1, GLfloat y1,GLfloat z1, GLfloat x2, GLfloat y2,GLfloat z2, GLfloat x3, GLfloat y3,GLfloat z3);
void bottleBezier();
static void resize(int width, int height);
void showControlPoints();
void display_obj(void);

#endif // CRV_H_INCLUDED
