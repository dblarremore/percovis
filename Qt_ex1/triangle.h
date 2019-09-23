//
//  OpenGL Triangle Widget
//

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QGLWidget>
#include <QString>

class Triangle : public QGLWidget
{
Q_OBJECT
public:
    Triangle(QWidget* parent=0);                      //  Constructor
    QSize sizeHint() const {return QSize(400,400);}   //  Default size of widget

public slots:
    void setTheta(int angle);                         //  Slot to set angle
    void setType(QString type);                       //  Slot to set color/texture

signals:
    void message(QString text);                       //  Signal for messages

protected:
    void initializeGL();                              //  Initialize widget
    void resizeGL(int width, int height);             //  Resize widget
    void paintGL();                                   //  Draw widget

private:
    int  darth;                                        //  Texture
    int  theta;                                        //  Rotation angle
    bool texture;                                      //  Color/Texture toggle
};

#endif
