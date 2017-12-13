#include <iostream>
#include <complex>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <math.h>
#include <time.h>
#include <QtOpenGL>
#include "Graph.h"
using namespace std;

/*
 * Constructor
 */
Graph :: Graph(QWidget* parent) : QGLWidget(parent)
{
	PI						= 3.14159265;
	timeHistory.push_back(0);
	GCHistory.push_back(0);
    aspectRatio             = 1;                // Aspect Ratio
    dim                     = 1;                // Dimension
}

/********************************************************************/
/*****************************  SETUP   *****************************/
/********************************************************************/

/*
 * Setup a new graph
 */
void Graph :: setupNewGraph()
{
	aFont.setPixelSize(16);
    aFont.setFamily("Helvetica");
}
/*
 * reset the graph
 */
void Graph :: reset()
{
    timeHistory.clear();
    GCHistory.clear();
    timeHistory.push_back(0);
    GCHistory.push_back(0);
    updateGL();
}

/********************************************************************/
/*****************************  DRAWING *****************************/
/********************************************************************/
/*
 * Render the data points int the data vectors
 */
void Graph :: drawGraph()
{
    glLineWidth(2.0);
    glColor3f(0.0,0.90,0.0);
    glBegin(GL_LINE_STRIP);
    for(unsigned int i=0; i<timeHistory.size(); ++i)
    {
        if(timeHistory[i] < 1)
        {
            glVertex2f(timeHistory[i],GCHistory[i]);
        }
    }
    glEnd();
}
/*
 * Render the axes
 */
void Graph :: drawAxes()
{
    glColor3f(1.0,1.0,1.0);
    glLineWidth(1.0);
    
    glBegin(GL_LINES);
    glVertex2f(0,0);
    glVertex2f(0,1);
    glVertex2f(0,0);
    glVertex2f(1,0);
    glEnd();
    renderText( 11, 4+aFont.pixelSize(),   "GC/N",     aFont,  2000 );
    renderText( viewWidth-60,   viewHeight-9,   "Links/N",  aFont,  2000 );
}

/********************************************************************/
/*****************************  DISPLAY *****************************/
/********************************************************************/
/*
 * Resize
 */
void Graph :: resizeGL(int width, int height)
{
    //  Window aspect ration
    viewWidth = width;
    viewHeight = height;
    // aspectRatio = (width && height) ? width / (float)height : 1;
    //  Viewport is whole screen
    glViewport(0,0,width,height);
    //  Set projection
    project();
}
/*
 *  Set projection
 */
void Graph :: project()
{
    //  Orthogonal projection to dim
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /*
     if (aspectRatio>1)
    {
        gluOrtho2D(-dim*aspectRatio, +dim*aspectRatio, -dim, +dim);
    }
    else
    {
        gluOrtho2D(-dim, +dim, -dim/aspectRatio, +dim/aspectRatio);
    }
     */
    
    //  Back to model view
    glMatrixMode(GL_MODELVIEW);
}
/*
 * Draws a circle of radius r, 0 is unfilled, 1 is filled
 */
void Graph :: drawCircle(float x, float y, float r, int isFilled)
{
    int numPoints = 25;
    if(isFilled)
    {
        glBegin(GL_POLYGON);
    }
    else
    {
        glBegin(GL_LINE_STRIP);
    }
    
    
    for(int i=0; i<numPoints+1; ++i)
    {
        glVertex2f(x+ r*cos(i*2.0*PI/numPoints),y+ r*sin(i*2.0*PI/numPoints));
    }
    glEnd();
}
/*
 * This function is called to display the screen
 */
void Graph :: initializeGL()
{
    // Do Nothing
}
/*
 * This function is called to display the screen
 */
void Graph :: paintGL()
{	
    
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT);
    // Reset transformations
    glLoadIdentity();
    
    //AntiAliasing
    glEnable (GL_LINE_SMOOTH);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    
    glPushMatrix();
    glTranslatef(-aspectRatio+0.05,-1.0+0.05,0.0);
    glScalef(2*(aspectRatio-0.05),1.9,1);
    
    drawAxes();
    drawGraph();
    glPopMatrix();
    
    // Flush and swap buffer
    glFlush();
}
/********************************************************************/
/************************** SLOTS ***********************************/
/********************************************************************/
/*
 * Receives a data point
 */
void Graph :: receiveDataPoint(float time,float GC)
{
    GCHistory.push_back(GC);
    timeHistory.push_back(time);
}

