//
//  OpenGL Triangle widget
//
#include <QtOpenGL>
#include "triangle.h"

//
//  Constructor
//
Triangle::Triangle(QWidget* parent)
    : QGLWidget(parent)
{
   //  Set intial display angle
   theta = 0;
   //  Initialize color/texture toggle
   texture = false;
}

//
//  Set theta (rotation angle)
//
void Triangle::setTheta(int angle)
{
   //  Set angle
   theta = angle;
   //  Request redisplay
   updateGL();
}

//
//  Set color or texture
//
void Triangle::setType(QString type)
{
   //  Set toggle
   texture = type.contains("Texture");
   //  Request redisplay
   updateGL();
}

//
//  Initialize
//
void Triangle::initializeGL()
{
   //  Enable Z-buffer depth testing
   glEnable(GL_DEPTH_TEST);
   //  Load Darth texture
   QPixmap img("darth.jpg");
   darth = bindTexture(img,GL_TEXTURE_2D);
}

//
//  Set projection when window is resized
//
void Triangle::resizeGL(int width, int height)
{
   //  Window aspect ration
   float asp= height ? width / (float)height : 1;

   //  Viewport is whole screen
   glViewport(0,0,width,height);

   //  Orthogonal projection to 1.5
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-1.5*asp, +1.5*asp, -1.5, +1.5, -1.5, +1.5);

   //  Back to model view
   glMatrixMode(GL_MODELVIEW);
}

//
//  Draw the window
//
void Triangle::paintGL()
{
    //  Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //  Reset transformations
    glLoadIdentity();

    //  Draw RGB triangle
    glBegin(GL_TRIANGLES);
    glColor3f(1,0,0); glVertex3d( 0, 1,0);
    glColor3f(0,1,0); glVertex3d(-1,-1,0);
    glColor3f(0,0,1); glVertex3d(+1,-1,0);
    glEnd();

    //  Set rotation
    glRotated(theta , 1,1,0);

    //  Draw CMY triangle
    if (texture)
    {
       glBindTexture(GL_TEXTURE_2D,darth);
       glEnable(GL_TEXTURE_2D);
       glColor3f(1,1,1);
       glBegin(GL_TRIANGLES);
       glTexCoord2d(0.5,1.0); glVertex3d( 0, 1,1);
       glTexCoord2d(0.0,0.0); glVertex3d(-1,-1,1);
       glTexCoord2d(1.0,0.0); glVertex3d(+1,-1,1);
       glEnd();
       glDisable(GL_TEXTURE_2D);
    }
    //  Draw textured triangle
    else
    {
       glBegin(GL_TRIANGLES);
       glColor3f(1,1,0); glVertex3d( 0, 1,1);
       glColor3f(0,1,1); glVertex3d(-1,-1,1);
       glColor3f(1,0,1); glVertex3d(+1,-1,1);
       glEnd();
    }

   //  Emit message to display
   emit message("Display at angle "+QString::number(theta)+" degrees");
}
