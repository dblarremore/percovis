#ifndef GRAPH_H
#define GRAPH_H

#include <QGLWidget>
#include <QString>
#include <vector>
using namespace std;

class Graph : public QGLWidget
{
	Q_OBJECT
private:
    float PI;
    vector<float> timeHistory;
    vector<float> GCHistory;
    float aspectRatio;
    float dim;
	QFont aFont;
    int viewWidth;
    int viewHeight;
public:
	Graph(QWidget* parent=0);
	QSize sizeHint() const {return QSize(400,200);}
    /*
     * Display Functions
     */
    void drawAxes();
    void drawGraph();
    void renderLabels();
    void setupNewGraph();
    void drawCircle(float,float,float,int);
    
public slots:
    void reset();
    void receiveDataPoint(float,float);
	
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int,int);
private:
    void project();

};

#endif
