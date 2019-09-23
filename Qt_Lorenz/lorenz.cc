//
//  OpenGL Lorenz Widget
//
#include <QtOpenGL>
#include <lorenz.h>

//
//  Constructor
//
Lorenz::Lorenz(QWidget* parent)
    : QGLWidget(parent)
{
   th = ph = 30;      //  Set intial display angles
   s  = 10;           //
   b  = 2.6666;       //  Initial parameters
   r  = 28;           //
   dt  = 0.001;       //  Time step
   asp = 1;           //  Aspect ratio
   dim = 50;          //  World dimension
   x0 = y0 = z0 = 1;  //  Starting location
   mouse = 0;         //  Mouse movement
}

/********************************************************************/
/*************************  Set parameters  *************************/
/********************************************************************/

//
//  Set r
//
void Lorenz::setR(double R)
{
   r = R;        //  Set parameter
   updateGL();   //  Request redisplay
}

//
//  Set b
//
void Lorenz::setB(double B)
{
   b = B;        //  Set parameter
   updateGL();   //  Request redisplay
}

//
//  Set s
//
void Lorenz::setS(double S)
{
   s = S;        //  Set parameter
   updateGL();   //  Request redisplay
}

//
//  Set dt
//
void Lorenz::setDT(double DT)
{
   dt = DT;      //  Set parameter
   updateGL();   //  Request redisplay
}

//
//  Set dim
//
void Lorenz::setDIM(double DIM)
{
   dim = DIM;    //  Set parameter
   project();
   updateGL();   //  Request redisplay
}

//
//  Set x0
//
void Lorenz::setX0(double X0)
{
   x0 = X0;      //  Set parameter
   updateGL();   //  Request redisplay
}

//
//  Set y0
//
void Lorenz::setY0(double Y0)
{
   y0 = Y0;      //  Set parameter
   updateGL();   //  Request redisplay
}

//
//  Set z0
//
void Lorenz::setZ0(double Z0)
{
   z0 = Z0;      //  Set parameter
   updateGL();   //  Request redisplay
}

//
//  Reset view angle
//
void Lorenz::reset(void)
{
   th = ph = 0;  //  Set parameter
   updateGL();   //  Request redisplay
}

/******************************************************************/
/*************************  Mouse Events  *************************/
/******************************************************************/
//
//  Mouse pressed
//
void Lorenz::mousePressEvent(QMouseEvent* e)
{
   mouse = true;
   pos = e->pos();  //  Remember mouse location
}

//
//  Mouse released
//
void Lorenz::mouseReleaseEvent(QMouseEvent*)
{
    mouse = false;
}

//
//  Mouse moved
//
void Lorenz::mouseMoveEvent(QMouseEvent* e)
{
   if (mouse)
   {
      QPoint d = e->pos()-pos;  //  Change in mouse location
      th = (th+d.x())%360;      //  Translate x movement to azimuth
      ph = (ph+d.y())%360;      //  Translate y movement to elevation
      pos = e->pos();           //  Remember new location
      updateGL();               //  Request redisplay
   }
}

//
//  Mouse wheel
//
void Lorenz::wheelEvent(QWheelEvent* e)
{
   //  Zoom out
   if (e->delta()<0)
      setDIM(dim+1);
   //  Zoom in
   else if (dim>2)
      setDIM(dim-1);
   //  Signal to change dimension spinbox
   emit dimen(dim);
}

/*******************************************************************/
/*************************  OpenGL Events  *************************/
/*******************************************************************/

//
//  Initialize
//
void Lorenz::initializeGL()
{
   glEnable(GL_DEPTH_TEST); //  Enable Z-buffer depth testing
   setMouseTracking(true);  //  Ask for mouse events
}

//
//  Window is resized
//
void Lorenz::resizeGL(int width, int height)
{
   //  Window aspect ration
   asp = (width && height) ? width / (float)height : 1;
   //  Viewport is whole screen
   glViewport(0,0,width,height);
   //  Set projection
   project();
}

//
//  Draw the window
//
void Lorenz::paintGL()
{
   //  Clear screen and Z-buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //  Reset transformations
   glLoadIdentity();
   //  Set rotation
   glRotated(ph , 1,0,0);
   glRotated(th , 0,1,0);

   //  Draw attractor
   double x = x0;
   double y = y0;
   double z = z0;

   //
   //  Integrate 50,000 steps (50 time units with dt = 0.001)
   //  Explicit Euler integration
   //
   /*  Colors */
   float R[] = {1.0,1.0,1.0,0.0,0.0,0.0};
   float G[] = {0.0,0.5,1.0,1.0,1.0,0.0};
   float B[] = {0.0,0.0,0.0,0.0,1.0,1.0};
   glBegin(GL_LINE_STRIP);
   for (int i=0;i<50000;i++)
   {
      double dx = s*(y-x);
      double dy = x*(r-z)-y;
      double dz = x*y - b*z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;
      glColor3f(R[i/10000],G[i/10000],B[i/10000]);
      glVertex3d(x,y,z);
   }
   glEnd();

   //
   //  Draw Axes
   //
   double len = 25;
   glColor3f(1,1,1);
   glBegin(GL_LINES);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(len,0.0,0.0);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(0.0,len,0.0);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(0.0,0.0,len);
   glEnd();
   //  Label Axes
   renderText(len,0.0,0.0,"X");
   renderText(0.0,len,0.0,"Y");
   renderText(0.0,0.0,len,"Z");

   //
   //  Emit signal with display angles and dimensions
   //
   emit angles("th,ph= "+QString::number(th)+","+QString::number(ph));

   //  Done
   glFlush();
}

//
//  Set projection
//
void Lorenz::project()
{
   //  Orthogonal projection to dim
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (asp>1)
      glOrtho(-dim*asp, +dim*asp, -dim, +dim, -3*dim, +3*dim);
   else
      glOrtho(-dim, +dim, -dim/asp, +dim/asp, -3*dim, +3*dim);

   //  Back to model view
   glMatrixMode(GL_MODELVIEW);
}
