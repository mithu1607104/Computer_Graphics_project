#include<GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include<math.h>
#include<bits/stdc++.h>
#include "crv.h"
using namespace std;

class BmpLoader
{
    public:
        unsigned char* textureData;
        int iWidth, iHeight;

        BmpLoader(const char*);
        ~BmpLoader();

    private:
        BITMAPFILEHEADER bfh;
        BITMAPINFOHEADER bih;
};

BmpLoader::~BmpLoader()//destructor
{
    delete [] textureData;
}

BmpLoader::BmpLoader(const char* filename)//constructor
{
    FILE *file=0;
    file=fopen(filename, "rb");
    if(!file)
        std::cout<<"File not found"<<std::endl;
    fread(&bfh, sizeof(BITMAPFILEHEADER),1,file);
    if(bfh.bfType != 0x4D42)
        std::cout<<"Not a valid bitmap"<<std::endl;
    fread(&bih, sizeof(BITMAPINFOHEADER),1,file);
    if(bih.biSizeImage==0)
        bih.biSizeImage=bih.biHeight*bih.biWidth*3;
    textureData = new unsigned char[bih.biSizeImage];/*take a memory*/
    fseek(file, bfh.bfOffBits, SEEK_SET);/*read actual image file*/
    fread(textureData, 1, bih.biSizeImage, file);
    unsigned char temp;
    for(int i=0; i<bih.biSizeImage; i+=3)/*convert textureData BGR to OpenGL RGB */
    {
        temp = textureData[i];
        textureData[i] = textureData[i+2];
        textureData[i+2] = temp;

    }

    iWidth = bih.biWidth;
    iHeight = bih.biHeight;
    fclose(file);
}


unsigned int ID[5];
double windowHeight=700, windowWidth=700;
const float asp_rat = 1.0 * windowHeight / windowWidth;
GLfloat theta = 0.0, axis_x=0.0, axis_y=0.0, eye_x = -60.0, eye_z = 60.0, eye_y = 30.0, ob_x=-40.0,ob_y=0.0,ob_z=40.0,look_x = ob_x, look_y = ob_y, look_z = ob_z;
GLfloat roomlength=100,roomwidth=100;
int  score=0;
float transpoint[27]={-40,10,-40,40,-20,20,-25,-30,-25,-10,-25,30,-20,41.5,-30,35,-30,-14.5,-30,-35,-20,-25,0,-30,-15,-40};
float scale[27]={20,7,1,20,7,1,20,7,1,10,7,1,10,7,1,10,7,1,20,7,1,0,7,12,0,7,12.5};
bool Collision_x_positive=false,Collision_x_negative=false,Collision_z_positive=false,Collision_z_negative=false;


GLboolean  light_01_on = true, light_02_on = true, light_03_on = true;
GLboolean amb_light = true, dif_light = true, spec_light = true;
bool score_obj_flag1=true,score_obj_flag2=true,t_flag=false;


static GLfloat v_cube[8][3] =
{
    {0,0,0},
    {0,0,1},
    {0,1,0},
    {0,1,1},

    {1,0,0},
    {1,0,1},
    {1,1,0},
    {1,1,1}
};

static GLubyte cube_Indices[6][4] =
{
    {3,1,5,7},  //front
    {6,4,0,2},  //back
    {2,3,7,6},  //top
    {1,0,4,5},  //bottom
    {7,5,4,6},  //right
    {2,0,1,3}   //left
};

static void getNormal3p
(GLfloat x1, GLfloat y1,GLfloat z1, GLfloat x2, GLfloat y2,GLfloat z2, GLfloat x3, GLfloat y3,GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}

void drawcube(double r, double g, double b, bool emission = false)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { r, g, b, 1.0 };
    GLfloat mat_diffuse[] = { r, g, b, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = {60};

    GLfloat mat_em[] = {1.0,1.0,1.0,1.0};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    if(emission) glMaterialfv( GL_FRONT, GL_EMISSION, mat_em);
    else glMaterialfv( GL_FRONT, GL_EMISSION, no_mat);

    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {
        getNormal3p(v_cube[cube_Indices[i][0]][0], v_cube[cube_Indices[i][0]][1], v_cube[cube_Indices[i][0]][2],
                    v_cube[cube_Indices[i][1]][0], v_cube[cube_Indices[i][1]][1], v_cube[cube_Indices[i][1]][2],
                    v_cube[cube_Indices[i][2]][0], v_cube[cube_Indices[i][2]][1], v_cube[cube_Indices[i][2]][2]);

        //for(int j = 0; j < 4; j++) glVertex3fv(&v_cube[cube_Indices[i][j]][0]);
        glTexCoord2f(1,1);glVertex3fv(&v_cube[cube_Indices[i][0]][0]);
        glTexCoord2f(1,0);glVertex3fv(&v_cube[cube_Indices[i][1]][0]);
        glTexCoord2f(0,0);glVertex3fv(&v_cube[cube_Indices[i][2]][0]);
        glTexCoord2f(0,1);glVertex3fv(&v_cube[cube_Indices[i][3]][0]);

    }
    glEnd();
}

static void res(int height, int width)
{
    float new_rat = 1.0 * height / width;
    float gap_side = (height/asp_rat) - width;
    float gap_up = (width*asp_rat) - height;
    if(new_rat > asp_rat) glViewport(gap_side/2,0,width*asp_rat, width);
    else glViewport(0,gap_up/2,height, height/asp_rat);
}




void light(double x, double y, double z, GLenum L, bool light_on, bool spot)
{
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[]  = {0.3, 0.3, 0.3, 1.0};
    GLfloat light_diffuse[]  = { 0.6, 0.6, 0.6, 1.0 };
    GLfloat light_specular[] = { 0.6, 0.6, 0.6, 1.0 };
    GLfloat light_position[] = { x, y, z, 1.0 };

    glEnable(L);

    if(light_on){

        if(amb_light) glLightfv( L, GL_AMBIENT, light_ambient);
        else glLightfv( L, GL_AMBIENT, no_light);

        if(dif_light) glLightfv( L, GL_DIFFUSE, light_diffuse);
        else glLightfv( L, GL_DIFFUSE, no_light);

        if(spec_light) glLightfv( L, GL_SPECULAR, light_specular);
        else glLightfv( L, GL_SPECULAR, no_light);

        glLightfv( L, GL_POSITION, light_position);
    }

    else{
        glLightfv( L, GL_AMBIENT, no_light);
        glLightfv( L, GL_DIFFUSE, no_light);
        glLightfv( L, GL_SPECULAR, no_light);
        glLightfv( L, GL_POSITION, no_light);
    }

    if(spot){
        GLfloat spot_direction[] = { 0.0, -1.0, 0.0 };
        glLightfv(L, GL_SPOT_DIRECTION, spot_direction);
        glLightf( L, GL_SPOT_CUTOFF, 55);
    }

}


void mazedraw()////*************
{
    //floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 2);
    glPushMatrix();
    glScalef(100.0, 0.5, 100.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //left wall
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 1);
    glPushMatrix();
    glTranslatef(-50, 0.0, -5);
    glScalef(1.0, 7.0, 90.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    //right wall
    glPushMatrix();
    glTranslatef(50, 0.0, 5.0);
    glScalef(1.0, 7.0, 90.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    //front wall
    glPushMatrix();
    glTranslatef(0.0, 0.0,-50.0);
    glScalef(100.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    //back_wall
    glPushMatrix();
    glTranslatef(0.0, 0.0,50.0);
    glScalef(100.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    ///inside walls

    glPushMatrix();///l1
    glTranslatef(-50.0+10.0, 0.0,10.0);
    glScalef(20.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///l2
    glTranslatef(-50.0+10.0, 0.0,-40.0);
    glScalef(20.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///l3
    glTranslatef(-20.0, 0.0,-20.0);
    glScalef(20.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///l4
    glTranslatef(-25.0, 0.0,-30.0);
    glScalef(10.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///l5
    glTranslatef(-25.0, 0.0,-10.0);
    glScalef(10.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///l6
    glTranslatef(-25.0, 0.0,30.0);
    glScalef(10.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///l7
    glTranslatef(-20.0, 0.0,41.5);
    glScalef(20.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///l6&l7_complete
    glTranslatef(-30.0, 0.0,35.0);
    glScalef(0.0, 7.0, 12.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///l3&l5
    glTranslatef(-30.0, 0.0,-14.5);
    glScalef(0.0, 7.0, 10.5);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///l2&l4
    glTranslatef(-30.0, 0.0,-35.0);
    glScalef(0.0, 7.0, 10.5);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///l3&l4
    glTranslatef(-20.0, 0.0,-25.0);
    glScalef(0.0, 7.0, 10.5);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///l8
    glTranslatef(0.0, 0.0,-30.0);
    glScalef(20.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///l9
    glTranslatef(-15.0, 0.0,-40.0);
    glScalef(10.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///l9&l8
    glTranslatef(-10.0, 0.0,-35.0);
    glScalef(0.0, 7.0, 10.5);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///l9&end z
    glTranslatef(-20.0, 0.0,-42.0);
    glScalef(0.0, 7.0, 5.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///from front wall to l9
    glTranslatef(-10.0, 0.0,-47.0);
    glScalef(0.0, 7.0, 5.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///L10
    glTranslatef(-5.0, 0.0,-45.0);
    glScalef(10.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///L10&l9
    glTranslatef(0.0, 0.0,-42.0);
    glScalef(0.0, 7.0, 5.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///f1_L10
    glTranslatef(10.0, 0.0,-47.0);
    glScalef(0.0, 7.0, 5.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///L11
    glTranslatef(27.0, 0.0,-44.0);
    glScalef(15.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///f1_L11
    glTranslatef(20.0, 0.0,-47.0);
    glScalef(0.0, 7.0, 5.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///r_L8
    glTranslatef(10.0, 0.0,-35.0);
    glScalef(0.0, 7.0, 10.5);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///L12
    glTranslatef(20.0, 0.0,-40.0);
    glScalef(20.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///middle_L12
    glTranslatef(15.0, 0.0,-42.0);
    glScalef(0.0, 7.0, 5.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///b_L8
    glTranslatef(10.0, 0.0,-25.0);
    glScalef(0.0, 7.0, 10.5);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///L13
    glTranslatef(5.0, 0.0,-20.0);
    glScalef(10.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///b_L13
    glTranslatef(0.0, 0.0,-15.0);
    glScalef(0.0, 7.0, 10.5);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    /*glPushMatrix();///L14
    //glTranslatef(-10.0, 0.0,-15.0);
    glScalef(20.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();*/


    glPushMatrix();///middle_L14
    glTranslatef(-10.0, 0.0,-12.0);
    glScalef(0.0, 7.0, 6.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();



    glPushMatrix();///parallel1_L1_L6
    glTranslatef(-10.0, 0.0,20.0);
    glScalef(0.0, 7.0, 20);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///b_L12
    glTranslatef(28.0, 0.0,-43.0);
    glScalef(0.0, 7.0, 5.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///L15
    glTranslatef(25.0, 0.0,-35.0);
    glScalef(20.0, 5.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///r_L15
    glTranslatef(35.0, 0.0,-37.0);
    glScalef(0.0, 7.0, 5.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///L16
    glTranslatef(42.0, 0.0,-40.0);
    glScalef(15.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///left_down_L15
    glTranslatef(15.0, 0.0,-25.0);
    glScalef(0.0, 7.0, 20.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

   /* glPushMatrix();///L17
   // glTranslatef(10.0, 0.0,-15.0);
    glScalef(10.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();*/


    glPushMatrix();///parallel2_L1 &Ll6
    glTranslatef(0.0, 0.0,25.0);
    glScalef(0.0, 7.0, 30.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///L18
    glTranslatef(5.0, 0.0,10.0);
    glScalef(10.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///r_L18
    glTranslatef(10.0, 0.0,20.0);
    glScalef(0.0, 7.0, 20);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///L19
    glTranslatef(14.0, 0.0,42.0);
    glScalef(10.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///b_L19_complete
    glTranslatef(19.0, 0.0,45.8);
    glScalef(0.0, 7.0, 9);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///back_corner
    glTranslatef(45.0, 0.0,45.8);
    glScalef(0.0, 7.0, 9);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///right_top_L19
    glTranslatef(19.0, 0.0,15.0);
    glScalef(0.0, 7.0, 9);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///L20
    glTranslatef(24.0, 0.0,10.0);
    glScalef(10.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///right_top_L20
    glTranslatef(29.0, 0.0,5.0);
    glScalef(0.0, 7.0, 9);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///right_top_L20_parallel
    glTranslatef(40.5, 0.0,5.0);
    glScalef(0.0, 7.0, 9);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///L21
    glTranslatef(45.0, 0.0,1.0);
    glScalef(9.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///right_down_L15
    glTranslatef(29.0, 0.0,-30.0);
    glScalef(0.0, 7.0, 9);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///perpendicular_right_down_L15
    glTranslatef(33.0, 0.0,-25.0);
    glScalef(9.0, 7.0, 0.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///L22
    glTranslatef(45.0, 0.0,30.5);
    glScalef(10.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///left_down L22
    glTranslatef(40.0, 0.0,35.0);
    glScalef(0.0, 7.0, 12.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///L23
    glTranslatef(35.0, 0.0,40.0);
    glScalef(12.0, 7.0, 0.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///left_top L23
    glTranslatef(30.0, 0.0,35.0);
    glScalef(0.0, 7.0, 12.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///right_top_L20_parallel
    glTranslatef(35.0, 0.0,12.0);
    glScalef(0.0, 7.0, 12);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();


    glPushMatrix();///L_24
    glTranslatef(30.0, 0.0,18.0);
    glScalef(12.0, 7.0, 0.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();




    glPushMatrix();///L17
    glTranslatef(5.0, 0.0,-10.0);
    glScalef(10.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();

    glPushMatrix();///L25
    glTranslatef(40.0, 0.0,-10.0);
    glScalef(20.0, 7.0, 1.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.5, 1.0, 0.5);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);



}

void objectdraw()///object that moves from source to destination
{
    glPushMatrix();
    //ob_x=-40;
    //ob_z=40;
    //glTranslatef(-40, 0.0, 40);
    glScalef(2.0, 2.0, 2.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(1.0, 1.0, 0.0);
    glPopMatrix();

}
void destinationwall()
{
    glPushMatrix();
    glTranslatef(50.0, 0.0,-45.0);
    glScalef(1.0, 7.0, 10.0);
    glTranslatef(-0.5,0.0,-0.5);
    drawcube(0.0, 1.0, 0.5);
    glPopMatrix();



}

void scoreobject1()///object that helps to increase score
{

 if(score_obj_flag1==true)
 {
    glPushMatrix();
    glTranslatef(0.0, 3.0, 0.0);
    //glScalef(2.0, 6.0, 2.0);
    glTranslatef(-0.5,0.0,-0.5);
    display_obj();
    glPopMatrix();
    destinationwall();



 }



         if(ob_x==0 && ob_z==0)///L1&L6
               {

                         glPushMatrix();
                         glTranslatef(0.0, -3.0, 0.0);

                         glTranslatef(-0.5,0.0,-0.5);
                         display_obj();
                         glPopMatrix();
                         ob_x++;
                         ob_x--;
                         score+=10;
                         score_obj_flag1=false;
                         //cout<<"SCORE IS|:"<<score<<endl;
                         if(score==20)
                         {
                          glPushMatrix();
                          glTranslatef(50.0, 0.0,-45.0);
                          glScalef(1.0, 7.0, 10.0);
                          glTranslatef(-0.5,0.0,-0.5);
                          drawcube(0.0, 1.0, 0.5);
                          glPopMatrix();


                         }


               }



}

void scoreobject2()
{
    if(score_obj_flag2==true)
    {
      glPushMatrix();
      glTranslatef(10.0, 3.0, 0.0);
    //glScalef(2.0, 6.0, 2.0);
      glTranslatef(-0.5,0.0,-0.5);
      display_obj();
      glPopMatrix();
      destinationwall();
    }


    if(ob_x==10 && ob_z==0)///L1&L6
               {

                         glPushMatrix();
                         glTranslatef(0.0, -3.0, 0.0);

                         glTranslatef(-0.5,0.0,-0.5);
                         display_obj();
                         glPopMatrix();
                         ob_x++;
                         ob_x--;
                         score+=10;
                         score_obj_flag2=false;

                         //cout<<"SCORE IS:"<<score<<endl;

                         if(score==20)
                         {
                          glPushMatrix();
                          glTranslatef(50.0, 0.0,-45.0);
                          glScalef(1.0, 7.0, 10.0);
                          glTranslatef(-0.5,0.0,-0.5);
                          drawcube(0.0, 1.0, 0.5);
                          glPopMatrix();


                         }



               }

}




void LoadTexture(const char*filename,int id)
{
    glGenTextures(1, &ID[id-1]);
    glBindTexture(GL_TEXTURE_2D, ID[id-1]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID[id-1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}
void output(int x, int y, float r, float g, float b, char *string)
{
  glColor3f( r, g, b );
  glOrtho(0,800,0,600,-5,5);
  glMatrixMode(GL_MODELVIEW);
  glRasterPos2i(x, y);
  int len, i;
  len = (int)strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
  }
  return;
}
void text()
{
   if(t_flag==true)
   {
    glPushMatrix();

    glTranslatef(50,10.0,-50.0);
    char str[80];
    sprintf(str, "Target Reached!");
    puts(str);
    output(0,0,1.0,1.0,0.0,str);
    glPopMatrix();
   }

}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);



    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum(-1.5,1.5,-1.5,1.5, 2, 500);


    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt(eye_x,eye_y,eye_z, look_x,look_y,look_z, 0,1,0);
    //gluLookAt(eye_x+ob_x,eye_y+ob_y,eye_z+ob_x, ob_x,ob_y,ob_z, 0,1,0);

    glRotatef( theta, 0.0, 1.0, 0.0);

    mazedraw();
    glPushMatrix();
    glTranslatef(ob_x,ob_y,ob_z);
    objectdraw();
    glPopMatrix();
    scoreobject1();
    scoreobject2();

    text();



    glDisable(GL_TEXTURE_2D);

    light(-8.0,8.0,0.0, GL_LIGHT0, light_01_on, false);
    light(12.0,8.0,5.0, GL_LIGHT1, light_02_on, false);
    light(5.0,6.0,1.0, GL_LIGHT2, light_03_on, true);


    glFlush();
    glutSwapBuffers();
}



void rotation_look()
{
    look_x-=eye_x;
    look_z-=eye_z;

    float r = sqrt(look_x*look_x + look_z*look_z);
    float theta_rad = atan(look_z/look_x);
    float conv = 180/3.1416;
    float theta_degree = theta_rad*conv;

    if(look_x < 0 ) theta_degree +=180;

    theta_degree+=1;
    theta_rad = theta_degree/conv;

    look_x = r*cos(theta_rad);
    look_z = r*sin(theta_rad);

    look_x+=eye_x;
    look_z+=eye_z;

}

void rotation_eye()
{
    eye_x-=look_x;
    eye_z-=look_z;

    float r = sqrt(eye_x*eye_x + eye_z*eye_z);
    float theta_rad = atan(eye_z/eye_x);
    float conv = 180/3.1416;
    float theta_degree = theta_rad*conv;

    if(eye_x < 0 ) theta_degree +=180;

    theta_degree+=1;
    theta_rad = theta_degree/conv;

    eye_x = r*cos(theta_rad);
    eye_z = r*sin(theta_rad);

    eye_x+=look_x;
    eye_z+=look_z;

}


void myKeyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case '+':
        eye_z--;
        look_z--;
        break;
    case '-':
        eye_z++;
        look_z++;
        break;

    case '4':
        eye_y--;
        look_y--;
        break;
    case '5':
        eye_y++;
        look_y++;
        break;

    case '6':
        eye_x--;
        look_x--;
        break;
    case '7':
        eye_x++;
        look_x++;
        break;

    case '8':
        look_y--;
        break;
    case '9':
        look_y++;
        break;

    case 'q':
        look_x--;
        break;
    case 'w':
        look_x++;
        break;

    case 'r':
        theta+=2.0;
        break;
    case 'l':
        theta-=2.0;
        break;


    case '1':
        light_01_on = !light_01_on;
        break;

    case '2':
        light_02_on = !light_02_on;
        break;

    case '3':
        light_03_on = !light_03_on;
        break;

    case 'a':
        amb_light = !amb_light;
        break;

    case 'd':
        dif_light = !dif_light;
        break;

    case 's':
        spec_light = !spec_light;
        break;

    case 'g':
        rotation_look();
        break;

    case 'h':
        rotation_eye();
        break;

    case 'M':



        if(ob_z<=(roomwidth/2)-2.25)///Boundary condition of -Z positive direction
        {
            if(Collision_z_negative==true)
            {
                for( int i=0,j=0;i<27 && j<27;i++,j++)
                {
                    if(ob_x==transpoint[i] && ob_z==scale[j])
                    {
                        ob_z=ob_z;//no movement
                    }
                }
            }
            else
            {
               ob_z++;
               eye_z++;
               look_z++;
            }






        }///backward moving of Z axes
        break;

    case 'N':


        if(ob_z>=-(roomwidth/2)+2.25)///Boundary condition of +Z positive direction
        {

            if(Collision_z_positive==true)
            {
                for( int i=0,j=0;i<27 && j<27;i++,j++)
                {
                    if(ob_x==transpoint[i] && ob_z==scale[j])
                    {
                        ob_z=ob_z;//no movement
                    }
                }
            }
            else
            {
                ob_z--;
                eye_z--;

                look_z--;
            }





        }///forward moving of Z axes
        break;

    case 'T':


        ///forward moving of x axes
        if(ob_x<=(roomlength/2)-2.25)///Boundary condition of +X positive direction
        {
            if(Collision_x_positive==true)
            {

                for( int i=0,j=0;i<27 && j<27;i++,j++)
                {
                    if(ob_x==transpoint[i] && ob_x==scale[j])
                    {
                        ob_x=ob_x;//no movement
                    }
                }

            }

           else
               {
                   if((ob_x<-32 || ob_z<29)||(ob_x<-32 || ob_z>43))
                   {
                      ob_x++;
                      eye_x++;
                     look_x++;

                   }



               }



        }



        break;
    case 'L':

        if(ob_x>=-(roomlength/2)+2.25)///Boundary condition of -X Negative direction
        {
            if(Collision_x_negative==true)
            {

                for( int i=0,j=0;i<27 && j<27;i++,j++)
                {
                    if(ob_x==transpoint[i] && ob_x==scale[j])
                    {
                        ob_x=ob_x;//no movement
                    }
                }

            }

            ob_x--;
            eye_x--;

            look_x--;

        }

         ///backward moving of x axes
        break;

    case 'X':
        if(ob_x>=(roomlength/2)-2.25)
        {
           ob_x++;
           t_flag=true;

        }

         break;


    case 27:
        exit(1);

  }
}



void animate()
{

    glutPostRedisplay();

}

void load_textures()
{
    LoadTexture("C:\\Users\\MITHU\\Desktop\\GP lab\\Final project\\brick.bmp",1);
    LoadTexture("C:\\Users\\MITHU\\Desktop\\GP lab\\Final project\\floor.bmp",2);


}

int main (int argc, char **argv)
{

    printf("Enter '+' to zoom in and '-' to zoom out\nEnter '4' to move the camera down and '5' to move the camera up.\n");
    printf("Enter '6' to move the camera left and '7' to move the camera right.\n");
    printf("Enter '8' to look at up and '9' to look at down.\n");
    printf("Enter 'q' to look at left and 'w' to look at right.\n");
    printf("Enter '1' to turn on or off light one, '2' to turn on or off light two and '3' to turn on or off light three\.n");
    printf("Enter 'a' to turn on or off ambient light, 'd' to turn on or off diffuse two and 's' to turn on or off specular light.\n");
    printf("Enter 'g' to rotate the look at point with respect to eye point.\n");
    printf("Enter 'h' to rotate the eye point with respect to look at point.\n");
    printf("Press T for forward moving in X direction\n Press L for backward moving in X direction\n Press N for forward moving in Z direction\nPress M for backward moving in Z direction\n");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(100,100);
    glutInitWindowSize(windowHeight, windowWidth);
    glutCreateWindow("Final project:1607104");
    glutReshapeFunc(res);

    load_textures();///load textures

    glShadeModel( GL_SMOOTH );
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);

    glutKeyboardFunc(myKeyboardFunc);
    glutDisplayFunc(display);
    glutIdleFunc(animate);
    glutMainLoop();

    return 0;
}

