//
//  OpenGL Lorenz Widget
//

#ifndef LORENZ_H
#define LORENZ_H

#include <QGLWidget>
#include <QString>

class Lorenz : public QGLWidget
{
Q_OBJECT                                             //  Qt magic macro
private:
   double s,b,r;     //  Lorenz attractor parameters
   double x0,y0,z0;  //  Start position
   int    th,ph;     //  Display angles
   double dt;        //  Integration time step
   bool   mouse;     //  Mouse pressed
   QPoint pos;       //  Mouse position
   double dim;       //  Display size
   double asp;       //  Sceen aspect ratio
public:
   Lorenz(QWidget* parent=0);                        //  Constructor
   QSize sizeHint() const {return QSize(400,400);}   //  Default size of widget

public slots:
    void setS(double S);        //  Slot to set s
    void setR(double R);        //  Slot to set r
    void setB(double B);        //  Slot to set b
    void setX0(double X0);      //  Slot to set x0
    void setY0(double Y0);      //  Slot to set y0
    void setZ0(double Z0);      //  Slot to set z0
    void setDT(double DT);      //  Slot to set dt
    void setDIM(double DIM);    //  Slot to set dim
    void reset(void);           //  Reset view angles

signals:
    void angles(QString text);  //  Signal for display angles
    void dimen(double dim);     //  Signal for display dimensions

protected:
    void initializeGL();                   //  Initialize widget
    void resizeGL(int width, int height);  //  Resize widget
    void paintGL();                        //  Draw widget
    void mousePressEvent(QMouseEvent*);    //  Mouse pressed
    void mouseReleaseEvent(QMouseEvent*);  //  Mouse released
    void mouseMoveEvent(QMouseEvent*);     //  Mouse moved
    void wheelEvent(QWheelEvent*);         //  Mouse wheel
private:
    void project();                        //  Set projection
};

#endif
