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
#include "Percnode.h"
#include "Cluster.h"
#include "Percolation.h"
using namespace std;

/*
 * Constructor
 */
Percolation :: Percolation(QWidget* parent) : QGLWidget(parent)
{
	PI						= 3.14159265;		// Pi
	N						= 150;              // Number of nodes
    newPercolationNodes     = 150;              // new Percolation nodes, 
	nodeRadius				= 1.5/N;			// Size of Nodes on screen
	springLength			= 3.0*nodeRadius;	// Resting length of springs
    coeffBoundaryStrength	= 4.0*N;			// Boundary strength coefficient
	coeffDrag				= 5.0;				// Fluid drag coefficient
	coeffElectrostatic		= 0.002;			// Electrostatic repulsion coefficient
	coeffSpring				= 14.0;				// Linear spring coefficient
	totalColors				= 0;                // Total number of colors
	T						= 0;				// Number of links added
    numberLinksAtOnce       = 1;                // Number of links to add at once
	timestep				= 1E-2;				// Integration timestep
	rule                    = "ER";				// Percolation rule
	GC						= 1;				// Size of Giant Component
	showInfo                = true;             // Show GC, T info.
    showProductRuleBrain    = false;            // Toggle PR brain
	showSocialClimberBrain	= false;			// Toggle SC brain
	fromSCBrain				= 0;				// SC's original from
	toSCBrain				= 0;				// SC's original to
	isPickyMeal				= true;				// For SC cluster consumption
	SC						= 1;				// Social Climber - {1,2}
    isSC                    = false;            // Social Climber on or off
	radius					= 0;				// Social Climber radius
	mouse					= false;			// Mouse Movement
    grabbedNode             = -1;               // Index of node grabbed by mouse
    
    aspectRatio             = 1;                // Aspect Ratio
    dim                     = 1;                // Dimension
    textSpacer              = 10;
    
    showSplash              = true;
    
    aFont.setPixelSize(16);
    aFont.setFamily("Helvetica");
    bFont.setPixelSize(40);
    bFont.setFamily("Helvetica");
}

/********************************************************************/
/*****************************  SETUP   *****************************/
/********************************************************************/

/*
 * Set Colors from 0 to 1 instead of 0 to 255
 */
void Percolation :: fixColors()
{
    float kolorz [] =  {100,100,100,	// grey 1
        255,165,0,		// orange 2
        255,69,0,		// red orange 3
        255,0,0,		// red 4
        255,20,150,		// magenta 5
        160,32,240,		// purple 6
        0,0,255,		// blue 7
        17,69,145,		// bluegreen 8
        34,139,34,		// green 9
        154,205,50,		// yellowgreen 10
        255,255,0,		// yellow 11
        155,155,0,		// dk yellow 12
        -1.0
    };
	int i=0;
	while(kolorz[i] != -1.0)
	{
		colors[i] = kolorz[i]/255.0;
		++i;
	}
	totalColors = i/3;
}
/*
 * Set up a new percolation
 */
void Percolation :: setupNewPercolation()
{
	// Fix the colors IF they haven't already been fixed.
	if(totalColors == 0)
	{
		fixColors();
	}
	// Seed the random number generators
	srand(time(NULL));
	srandom(time(NULL));
	
	// Make some nodes and stuff
	for (unsigned int i=0; i<N; ++i)
	{
		Cluster *newCluster = new Cluster(i);
		netNodes.push_back(new Percnode(newCluster,i));
		newCluster->initializeCluster(netNodes[i]);
		float angle = i*7.0*PI/N;
        float radius = sin(5.0/7.0*angle);
		netNodes[i]->setX(0.5+0.4*radius*cos(angle+PI));
		netNodes[i]->setY(0.55+0.4*radius*sin(angle+PI));
	}
	// Set up the distances matrix for the first time
	updateDistances();
}
/*
 * reset the percolation
 */
void Percolation :: reset()
{
    N=newPercolationNodes;
    T=0;
    GC=1;
    nodeRadius				= 0.25/pow(N,0.6);			// Size of Nodes on screen
	springLength			= 3.5*nodeRadius;	// Resting length of springs
    coeffBoundaryStrength	= 4.0*N;			// Boundary strength coefficient
    netNodes.clear();
	for (unsigned int i=0; i<N; ++i)
	{
		Cluster *newCluster = new Cluster(i);
		netNodes.push_back(new Percnode(newCluster,i));
		newCluster->initializeCluster(netNodes[i]);
        float angle = i*4.0*PI/N;
        float radius = i*1.0/(N+2)/2;
		netNodes[i]->setX(0.5+radius*cos(angle));
		netNodes[i]->setY(0.5+radius*sin(angle));
	}
    percolationHistory.clear();
    fromHistory.clear();
    toHistory.clear();
    timeSeriesGC.clear();
	// Set up the distances matrix for the first time
    
    emit resetGraph();
    
	updateDistances();
    updateGL();
}

/********************************************************************/
/*****************************  DISPLAY CALCULATIONS ****************/
/********************************************************************/

/*
 * Update the distance matrix
 */
void Percolation :: updateDistances()
{
	float xi=0.0;
	float yi=0.0;
	float xj=0.0;
	float yj=0.0;
	for (unsigned int i=0; i<N; ++i)
	{
		xi=netNodes[i]->getX();
		yi=netNodes[i]->getY();
		for (unsigned int j=i+1; j<N; ++j)
		{
			xj=netNodes[j]->getX();
			yj=netNodes[j]->getY();
			// X distances
			distances[0][i][j] = xi-xj;
			distances[0][j][i] = -distances[0][i][j];
			// Y distances
			distances[1][i][j] = yi-yj;
			distances[1][j][i] = -distances[1][i][j];
			// Cartesian distances
			distances[2][i][j] = sqrt(distances[0][i][j]*distances[0][i][j]+distances[1][i][j]*distances[1][i][j]);
			distances[2][j][i] = distances[2][i][j];
		}
	}
}
/*
 * Numerical integration, updating X and Y coordinates every timestep
 */
void Percolation :: updateXY()
{
	updateDistances();
    float sumElectrostaticTermX;
	float sumElectrostaticTermY;
	float sumSpringTermX;
	float sumSpringTermY;
	float xi;
	float yi;
	float distanceToCenter;
    float xNew;
    float yNew;
    float dxNew;
    float dyNew;
	// go through and update all DX, X, DY, Y
	for (unsigned int i=0; i<N; ++i)
	{
        if(signed(i) != grabbedNode)
        {
            sumElectrostaticTermX=0.0;
            sumElectrostaticTermY=0.0;
            sumSpringTermX=0.0;
            sumSpringTermY=0.0;
            for(unsigned int j=0; j<N; ++j)
            {
                if(i != j)
                {
                    sumElectrostaticTermX += distances[0][i][j] / pow(distances[2][i][j],3);
                    sumElectrostaticTermY += distances[1][i][j] / pow(distances[2][i][j],3);
                }
            }
            xi = netNodes[i]->getX();
            yi = netNodes[i]->getY();
            distanceToCenter = sqrt((xi-0.5)*(xi-0.5) + (yi-0.5)*(yi-0.5));
            
            sumElectrostaticTermX += coeffBoundaryStrength*((0.5-xi)/pow(1-distanceToCenter,3));
            sumElectrostaticTermY += coeffBoundaryStrength*((0.5-yi)/pow(1-distanceToCenter,3));
            
            for(unsigned int j=0; j<netNodes[i]->getNeighbors()->size(); ++j)
            {
                sumSpringTermX += (1-springLength/distances[2][i][(*netNodes[i]->getNeighbors())[j]->getID()]) 
                * ((*netNodes[i]->getNeighbors())[j]->getX()-xi);
                sumSpringTermY += (1-springLength/distances[2][i][(*netNodes[i]->getNeighbors())[j]->getID()]) 
                * ((*netNodes[i]->getNeighbors())[j]->getY()-yi);
            }
            
            dxNew = netNodes[i]->getDX() + timestep * (-coeffDrag*netNodes[i]->getDX() + coeffElectrostatic*sumElectrostaticTermX + coeffSpring*sumSpringTermX);
            // Can't go faster than 1
            if(dxNew > -1 && dxNew < 1)
            {
                netNodes[i]->setDX(dxNew);
            }
            else
            {
                netNodes[i]->setDX(dxNew/fabs(dxNew));
            }
            xNew = netNodes[i]->getX()  + timestep * netNodes[i]->getDX();
            // Never let a node leave the circle
            if(xNew > 0 && xNew < 1)
            {
                netNodes[i]->setX(xNew);
            }
            else
            {
                netNodes[i]->setDX(0);
            }
            
            dyNew = netNodes[i]->getDY() + timestep * (-coeffDrag*netNodes[i]->getDY() + coeffElectrostatic*sumElectrostaticTermY + coeffSpring*sumSpringTermY);
            // Can't go faster than 1
            if(dyNew > -1 && dyNew < 1)
            {
                netNodes[i]->setDY(dyNew);
            }
            else
            {
                netNodes[i]->setDY(dyNew/fabs(dyNew));
            }
            yNew = netNodes[i]->getY()  + timestep * netNodes[i]->getDY();
            // Never let a node leave the circle
            if (yNew > 0 && yNew < 1)
            {
                netNodes[i]->setY(yNew);
            }
            else
            {
                netNodes[i]->setDY(0);
            }
        }
    }
    updateGL();
}
/*
 * Mouse Pressed
 */
void Percolation :: mousePressEvent(QMouseEvent* e)
{
    mouse = true;
    pos = e->pos();  //  Remember mouse location
    // Find which node was grabbed
    if(aspectRatio > 1)
    {
        mouseXGL = (2.0*pos.x() - viewWidth + viewHeight)/(2.0*viewHeight);
        mouseYGL = (1.0 - pos.y()/float(viewHeight));
    }
    else
    {
        mouseXGL = pos.x()/float(viewWidth);
        mouseYGL = (viewHeight + viewWidth - 2.0*pos.y())/(2.0*viewWidth);   
    }
    for(unsigned int i=0; i<N; ++i)
    {
        if(fabs(netNodes[i]->getX()-mouseXGL) < 1.5*nodeRadius
           &&
           fabs(netNodes[i]->getY()-mouseYGL) < 1.5*nodeRadius)
        {
            grabbedNode = i;
        }
    }
}
/*
 * Mouse Released
 */
void Percolation :: mouseReleaseEvent(QMouseEvent*)
{
    mouse = false;
    grabbedNode = -1;
}
/*
 * Mouse Moved
 */
void Percolation :: mouseMoveEvent(QMouseEvent* e)
{
    if (mouse && grabbedNode > -1)
    {
        float translatedX;
        float translatedY;
        if(aspectRatio > 1)
        {
            translatedX = (2.0*e->pos().x() - viewWidth + viewHeight)/(2.0*viewHeight);
            translatedY = (1.0 - e->pos().y()/float(viewHeight));
        }
        else
        {
            translatedX = e->pos().x()/float(viewWidth);
            translatedY = (viewHeight + viewWidth - 2.0*e->pos().y())/(2.0*viewWidth);
        }
        if(translatedX > 1)
        {
            translatedX = 1;
        }
        else if(translatedX < 0)
        {
            translatedX = 0;
        }
        if(translatedY > 1)
        {
            translatedY = 1;
        }
        if(translatedY < 0)
        {
            translatedY = 0;
        }
        netNodes[grabbedNode]->setX(translatedX); // move the node
        netNodes[grabbedNode]->setY(translatedY); // move the node
        updateGL();               //  Request redisplay
    }
}
/********************************************************************/
/*****************************  DISPLAY *****************************/
/********************************************************************/
/*
 * Resize
 */
void Percolation :: resizeGL(int width, int height)
{
    //  Window aspect ration
    viewWidth = width;
    viewHeight = height;
    aspectRatio = (width && height) ? width / (float)height : 1;
    //  Viewport is whole screen
    glViewport(0,0,width,height);
    
    //  Set projection
    project();
}
/*
 *  Set projection
 */
void Percolation :: project()
{
    //  Orthogonal projection to dim
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (aspectRatio>1)
    {
        //gluOrtho2D(-dim*aspectRatio, +dim*aspectRatio, -dim, +dim);
        glOrtho(-dim*aspectRatio, +dim*aspectRatio, -dim, +dim,-1.0,1.0);
    }
    else
    {
        //gluOrtho2D(-dim, +dim, -dim/aspectRatio, +dim/aspectRatio);
        glOrtho(-dim, +dim, -dim/aspectRatio, +dim/aspectRatio,-1.0,1.0);
    }
    
    //  Back to model view
    glMatrixMode(GL_MODELVIEW);
}
/*
 * Prints GC size and color codes the text
 */
void Percolation :: printGC()
{
    glColor3f(1.0,1.0,1.0);
    renderText(textSpacer,viewHeight-textSpacer, "Giant Component", aFont, 2000 );
    QString GCString;
    GCString.setNum(GC);
    if(GC >= totalColors)
    {
        glColor3f(1.0,1.0,1.0);
    }
    else
    {
        glColor3f(colors[GC*3-3],colors[GC*3-2],colors[GC*3-1]);
    }
    renderText(3*textSpacer,viewHeight-2*textSpacer-aFont.pixelSize(),GCString,bFont, 2000);
    
}
/*
 * Prints GC size and color codes the text
 */
void Percolation :: printTime()
{
    glColor3f(1.0,1.0,1.0);
    renderText(textSpacer,textSpacer+aFont.pixelSize(),"Links Added",aFont, 2000);
    QString TString;
    TString.setNum(percolationHistory.size());
    renderText(3*textSpacer,2*textSpacer+aFont.pixelSize()+bFont.pixelSize(),TString,bFont,2000);
}
/*
 * Print Rule
 */
void Percolation :: printRule()
{
    glColor3f(1.0,1.0,1.0);
    int baseTextLengthA = 118;
    int baseTextLengthB = 54;
    renderText(viewWidth-baseTextLengthA-textSpacer,viewHeight-textSpacer,"Percolation Rule",aFont,2000);
    if (rule.compare("AE")==0)
    {
        renderText(viewWidth-baseTextLengthB-3*textSpacer,viewHeight-2*textSpacer-aFont.pixelSize(),"AE",bFont,2000);
    }
    else if (rule.compare("ER")==0)
    {
        renderText(viewWidth-baseTextLengthB-3*textSpacer,viewHeight-2*textSpacer-aFont.pixelSize(),"ER",bFont,2000);
    }
    else if (rule.compare("PR")==0)
    {
        renderText(viewWidth-baseTextLengthB-3*textSpacer,viewHeight-2*textSpacer-aFont.pixelSize(),"PR",bFont,2000);
    }
}
/*
 * Print SC
 */
void Percolation :: printSC()
{
    glColor3f(1.0,1.0,1.0);
    int baseTextLengthA = 105;
    int baseTextLengthB = 78;
    int baseTextLengthC = 21;
    int baseTextLengthD = 19;
    renderText(viewWidth-baseTextLengthA-textSpacer,aFont.pixelSize()+textSpacer,"Social Climber",aFont,2000);
    if (!isSC)
    {
        renderText(viewWidth-baseTextLengthB-3*textSpacer,aFont.pixelSize()+bFont.pixelSize()+2*textSpacer,"OFF",bFont,2000);
    }
    else
    {
        if (SC==1)
        {
            renderText(viewWidth-baseTextLengthC-3*textSpacer-baseTextLengthD,aFont.pixelSize()+bFont.pixelSize()+2*textSpacer,"1",bFont,2000);
            QString radiusString;
            radiusString.setNum(radius);
            radiusString.prepend("(");
            radiusString.append(")");
            renderText(viewWidth-baseTextLengthD-3*textSpacer,aFont.pixelSize()+0.75*bFont.pixelSize()+2*textSpacer,radiusString,aFont,2000);
        }
        else 
        {
            renderText(viewWidth-baseTextLengthC-3*textSpacer-baseTextLengthD,aFont.pixelSize()+bFont.pixelSize()+2*textSpacer,"2",bFont,2000);
            QString radiusString;
            radiusString.setNum(radius);
            radiusString.prepend("(");
            radiusString.append(")");
            renderText(viewWidth-baseTextLengthD-3*textSpacer,aFont.pixelSize()+0.75*bFont.pixelSize()+2*textSpacer,radiusString,aFont,2000);
        }
    }
}
/*
 * Draws a link from node A to node B
 */
void Percolation :: drawLink(int A, int B)
{
    glPushMatrix();
    glTranslatef(-1.0,-1.0,0.0);
    glScalef(2.0,2.0,2.0);
    
    // Render the link
    glBegin(GL_LINES);
    glVertex2f(netNodes[A]->getX(),netNodes[A]->getY());
    glVertex2f(netNodes[B]->getX(),netNodes[B]->getY());
    glEnd();
    
    glPopMatrix();
}
/*
 * Draws a circle of radius r, 0 is unfilled, 1 is filled
 */
void Percolation :: drawCircle(float x, float y, float r, int isFilled)
{
    // Number of points to use to draw the circle
    int numPoints = 100;
    
    
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
 * Renders all the links in percolationHistory
 */
void Percolation :: renderLinks()
{
    glLineWidth(1.0);
    glColor3f(1.0,1.0,1.0);
    for (unsigned int i=0; i < percolationHistory.size(); ++i)
    {
        if(i == percolationHistory.size() - 1 && !showSplash)
        {
            glColor3f(0.0,1.0,0.0);
            glLineWidth(2.0);
        }
        drawLink(percolationHistory[i] % N,percolationHistory[i] / N);
    }
}
/*
 * Renders product rule brain links
 */
void Percolation :: renderProductRuleLinks()
{
    glLineWidth(2.0);
    glColor3f(1.0,1.0,0.0);
    glLineStipple(4, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    drawLink(fromPRBrain,toPRBrain);
    glDisable(GL_LINE_STIPPLE);
}
/*
 * Renders social climber brain links
 */
void Percolation :: renderSocialClimberLinks()
{
    glLineWidth(2.0);
    glColor3f(1.0,0.0,0.0);
    glLineStipple(4, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    drawLink(fromSCBrain,toSCBrain);
    glDisable(GL_LINE_STIPPLE);
}
/*
 * Renders all the nodes, color coding for cluster size
 */
void Percolation :: renderNodes()
{
    glPushMatrix();
    glTranslatef(-1.0,-1.0,0.0);
    glScalef(2.0,2.0,2.0);
    float colorA;
    float colorB;
    float colorC;
    int clusterSize;
    bool isGC;
    for(unsigned int i=0; i<N; ++i)
    {
        isGC = 0;
        clusterSize = netNodes[i]->getCluster()->getSize();
        if (clusterSize == GC && clusterSize != 1)
        {
            isGC = 1;
        }
        clusterSize--;
        clusterSize = min(clusterSize,totalColors-1);
        colorA = colors[clusterSize*3+0];
        colorB = colors[clusterSize*3+1];
        colorC = colors[clusterSize*3+2];
        if(showSplash)
        {
            float angle = i*7.0*PI/N;
            float radius = fabs(sin(5.0/7.0*angle));
            colorA = 0.8-0.5*radius;
            colorB = colorA;
            colorC = colorA;
        }
        
        if(isGC && GC < 12)
        {
            glColor3f(colorA*0.3,colorB*0.3,colorC*0.3);
            if(netNodes[i]->getCluster()->getMaxDegreeNode() == netNodes[i])
            {
                glColor3f(colorA*0.5,colorB*0.5,colorC*0.5);
            }
            drawCircle(netNodes[i]->getX(),netNodes[i]->getY(),1.25*nodeRadius,1);
            glColor3f(1.0,1.0,1.0);
            glLineWidth(3.0);
            drawCircle(netNodes[i]->getX(),netNodes[i]->getY(),1.35*nodeRadius,0);
            glLineWidth(2.0);
            glColor3f(colorA,colorB,colorC);
            drawCircle(netNodes[i]->getX(),netNodes[i]->getY(),1.3*nodeRadius,0);
        }
        else if(isGC && GC >=12)
        {
            glColor3f(0.3,0.3,0.3);
            if(netNodes[i]->getCluster()->getMaxDegreeNode() == netNodes[i])
            {
                glColor3f(0.5,0.5,0.5);
            }
            drawCircle(netNodes[i]->getX(),netNodes[i]->getY(),1.25*nodeRadius,1);
            glColor3f(1.0,1.0,1.0);
            glLineWidth(3.0);
            drawCircle(netNodes[i]->getX(),netNodes[i]->getY(),1.35*nodeRadius,0);
            glLineWidth(2.0);
            drawCircle(netNodes[i]->getX(),netNodes[i]->getY(),1.3*nodeRadius,0);
        }
        else
        {
            glColor3f(colorA*0.3,colorB*0.3,colorC*0.3);
            if(netNodes[i]->getCluster()->getMaxDegreeNode() == netNodes[i])
            {
                glColor3f(colorA*0.5,colorB*0.5,colorC*0.5);
            }
            drawCircle(netNodes[i]->getX(),netNodes[i]->getY(),nodeRadius,1);
            glLineWidth(2.0);
            glColor3f(colorA,colorB,colorC);
            drawCircle(netNodes[i]->getX(),netNodes[i]->getY(),nodeRadius,0);
        }
        if(signed(i)==grabbedNode)
        {
            glLineWidth(1.0);
            glColor3f(1.0,1.0,1.0);
            drawCircle(netNodes[i]->getX(),netNodes[i]->getY(),nodeRadius/2,1);
            glColor3f(0.0,0.0,0.0);
            drawCircle(netNodes[i]->getX(),netNodes[i]->getY(),nodeRadius/2,0);
        }
    }
    glPopMatrix();
}
/*
 * This function is called to display the screen
 */
void Percolation :: initializeGL()
{
    setMouseTracking(true);  //  Ask for mouse events
}
/*
 * This function makes a pretty splash
 */
void Percolation :: splash()
{
    int textWidthB = 500;
    int textWidthC = 198;
    int textWidthD = 224;
    // P E R C O V I S
    splashFont.setPixelSize(120);
    splashFont.setFamily("Helvetica");
    
    glColor3f(255.0/255,20.0/255,147.0/255);
    renderText(viewWidth/2-textWidthB/2-2,  0.4*viewHeight+splashFont.pixelSize()/2-2,    "PercoVIS", splashFont, 2000);
    
    glColor3f(0.0,0.8,0.0);
    renderText(viewWidth/2-textWidthB/2,  0.4*viewHeight+splashFont.pixelSize()/2,    "PercoVIS", splashFont, 2000);
    
    // Copyright
    glColor3f(1,1,1);
    renderText(viewWidth/2-textWidthC/2, viewHeight-29, "Copyright Daniel Larremore", aFont, 2000);
    renderText(viewWidth/2-textWidthD/2, viewHeight-10, "daniel.larremore@colorado.edu", aFont, 2000);
}
/*
 * This function is called to display the screen
 */
void Percolation :: paintGL()
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
    
    if(showSplash)
    {  
        renderNodes();
        splash();
    }
    else
    {
        if(showSocialClimberBrain && isSC && percolationHistory.size() > 0)
        {
            renderSocialClimberLinks();
        }
        if(showProductRuleBrain && percolationHistory.size() > 0 && rule.compare("PR")==0)
        {
            renderProductRuleLinks();
        }
        renderLinks();
        renderNodes();
        if(showInfo)
        {
            printGC();
            printRule();
            // printSC(); //DISABLED
            printTime();
        }
    }
    // Flush and swap buffer
    glFlush();
}
/********************************************************************/
/************************** PERCOLATION *****************************/
/********************************************************************/
/*
 * does one step of percolation process
 */
void Percolation :: percolate()
{
    //long int getEdge(string rule, int N, vector<Percnode*> *netNodes, long int PRErain*);
    edge = getEdge(rule,N,&netNodes,&PREdge);
    if (rule.compare("PR")==0)
    {
        fromPRBrain = PREdge % N;
        toPRBrain = PREdge / N;
    }
    from = edge % N;
    to = edge / N;
    isPickyMeal=0;
    fromSCBrain=from;
    toSCBrain=to;
    
    //Have one cluster eat the other one IF they are different
    if(netNodes[from]->getCluster() != netNodes[to]->getCluster())
    {
        //Social Climber:
        if(isSC)
        {
            // SC == 1 only social climber the "to" connection
            if(SC == 1)
            {
                // Simple Case: infinite radius
                if(radius == 0)
                {
                    // If "to" isn't the largest degree in the cluster, change "to"
                    if(netNodes[to] != netNodes[to]->getCluster()->getMaxDegreeNode())
                    {
                        to=netNodes[to]->getCluster()->getMaxDegreeNode()->getID();
                    }
                }
                // Complex Case: finite radius
                else
                {
                    // Simplification - if radius > clusterSize, simple case works
                    if(radius > netNodes[to]->getCluster()->getSize())
                    {
                        if(netNodes[to] != netNodes[to]->getCluster()->getMaxDegreeNode())
                        {
                            to=netNodes[to]->getCluster()->getMaxDegreeNode()->getID();
                        }
                    }
                    // Dirty work
                    // Make a list of: to, nbrs(to), nbrs(nbrs(to))...etc, radius depth.
                    else
                    {
                        // declare a vector of potential nodes to connect to
                        vector<Percnode*> candidates;
                        // add to to the list
                        candidates.push_back(netNodes[to]);
                        // take him out as an additional candidate
                        netNodes[to]->makeIneligible();
                        // state that you're going to search radius-1 levels after this call
                        int levelsToSearch = radius;
                        // now get the tree of neighbors
                        netNodes[to]->nominateNeighbors(&candidates, &levelsToSearch);
                        
                        // Take a first guess that to is your best bet and his degree is zero
                        // We KNOW this to be not true, but leaving these uninitialized may be undesirable
                        Percnode* bestCandidate = netNodes[to];
                        unsigned int largestDegreeSoFar=0;
                        // candidates is now populated, so go through it, and get the max degree guy
                        // as you go, reset eligibility for the future.
                        for(unsigned int i=0; i < candidates.size(); ++i)
                        {
                            if(candidates[i]->getDegree() > largestDegreeSoFar)
                            {
                                largestDegreeSoFar = candidates[i]->getDegree();
                                bestCandidate = candidates[i];
                            }
                            candidates[i]->makeEligible();
                        }
                        
                        //good work! You have socially climbed.
                        to = bestCandidate->getID();
                        // Make sure that you have a picky feast.
                        isPickyMeal=1;
                    }
                }
            }
            // SC == 2 then social climber "to" and "from" connections
            else if(SC == 2)
            {
                // Simple Case: infinite radius
                if(radius == 0)
                {
                    // If "to" isn't the largest degree in the cluster, change "to"
                    if(netNodes[to] != netNodes[to]->getCluster()->getMaxDegreeNode())
                    {
                        to=netNodes[to]->getCluster()->getMaxDegreeNode()->getID();
                    }
                    // If "from" isn't the largest degree in the cluster, change "from"
                    if(netNodes[from] != netNodes[from]->getCluster()->getMaxDegreeNode())
                    {
                        from=netNodes[from]->getCluster()->getMaxDegreeNode()->getID();
                    }
                }
                // Complex Case: finite radius
                else
                {
                    
                    // complex social climb TO
                    
                    // Simplification - if radius > clusterSize, simple case works
                    if(radius > netNodes[to]->getCluster()->getSize())
                    {
                        if(netNodes[to] != netNodes[to]->getCluster()->getMaxDegreeNode())
                        {
                            to=netNodes[to]->getCluster()->getMaxDegreeNode()->getID();
                        }
                    }
                    // Dirty work
                    // Make a list of: to, nbrs(to), nbrs(nbrs(to))...etc, radius depth.
                    else
                    {
                        // declare a vector of potential nodes to connect to
                        vector<Percnode*> candidates;
                        // add to to the list
                        candidates.push_back(netNodes[to]);
                        // take him out as an additional candidate
                        netNodes[to]->makeIneligible();
                        // state that you're going to search radius-1 levels after this call
                        int levelsToSearch = radius;
                        // now get the tree of neighbors
                        netNodes[to]->nominateNeighbors(&candidates, &levelsToSearch);
                        
                        // Take a first guess that to is your best bet and his degree is zero
                        // We KNOW this to be not true, but leaving these uninitialized may be undesirable
                        Percnode* bestCandidate = netNodes[to];
                        unsigned int largestDegreeSoFar=0;
                        // candidates is now populated, so go through it, and get the max degree guy
                        // as you go, reset eligibility for the future.
                        for(unsigned int i=0; i < candidates.size(); ++i)
                        {
                            if(candidates[i]->getDegree() > largestDegreeSoFar)
                            {
                                largestDegreeSoFar = candidates[i]->getDegree();
                                bestCandidate = candidates[i];
                            }
                            candidates[i]->makeEligible();
                        }
                        
                        //good work! You have socially climbed.
                        to = bestCandidate->getID();	
                        
                        // Make sure that you have a picky feast.
                        isPickyMeal=1;
                    }
                    
                    
                    //  complex social climb FROM
                    // Simplification - if radius > clusterSize, simple case works
                    if(radius > netNodes[from]->getCluster()->getSize())
                    {
                        if(netNodes[from] != netNodes[from]->getCluster()->getMaxDegreeNode())
                        {
                            from=netNodes[from]->getCluster()->getMaxDegreeNode()->getID();
                        }
                    }
                    // Dirty work
                    // Make a list of: from, nbrs(from), nbrs(nbrs(from))...etc, radius depth.
                    else
                    {
                        // declare a vector of potential nodes to connect to
                        vector<Percnode*> candidates;
                        // add to to the list
                        candidates.push_back(netNodes[from]);
                        // take him out as an additional candidate
                        netNodes[from]->makeIneligible();
                        // state that you're going to search radius-1 levels after this call
                        int levelsToSearch = radius;
                        // now get the tree of neighbors
                        netNodes[from]->nominateNeighbors(&candidates, &levelsToSearch);
                        
                        // Take a first guess that to is your best bet and his degree is zero
                        // We KNOW this to be not true, but leaving these uninitialized may be undesirable
                        Percnode* bestCandidate = netNodes[from];
                        unsigned int largestDegreeSoFar=0;
                        // candidates is now populated, so go through it, and get the max degree guy
                        // as you go, reset eligibility for the future.
                        for(unsigned int i=0; i < candidates.size(); ++i)
                        {
                            if(candidates[i]->getDegree() > largestDegreeSoFar)
                            {
                                largestDegreeSoFar = candidates[i]->getDegree();
                                bestCandidate = candidates[i];
                            }
                            candidates[i]->makeEligible();
                        }
                        
                        //good work! You have socially climbed.
                        from = bestCandidate->getID();	
                        
                        // Make sure that you have a picky feast.
                        isPickyMeal=1;
                    }
                }
            }
        }
        //have the "from" cluster eat the "to" cluster;
        Cluster *oldCluster = netNodes[to]->getCluster();
        if(isPickyMeal)
        {
            netNodes[from]->getCluster()->pickyEatCluster(oldCluster);
        }
        else
        {
            netNodes[from]->getCluster()->eatCluster(oldCluster);
        }
        
        //delete the oldCluster from memory
        delete oldCluster;		
        
        //Update the GC if the new cluster is bigger than the GC was.
        GC = max(GC,netNodes[from]->getCluster()->getSize());
    }
    else
    {
        //Just keep the "to" and "from" the way that they are and DO NOT SOCIAL CLIMB.
        //No clusters will be merged.
    }
    
    //Check to see if the max degree of the cluster should be updated to one of the freshly linked nodes.
    unsigned int oldMaxDegree = netNodes[from]->getCluster()->getMaxDegreeNode()->getDegree();
    if(oldMaxDegree < netNodes[from]->getDegree()+1)
    {
        netNodes[from]->getCluster()->setMaxDegreeNode(netNodes[from]);
    }
    else if(oldMaxDegree < netNodes[to]->getDegree()+1)
    {
        netNodes[to]->getCluster()->setMaxDegreeNode(netNodes[to]);
    }
    
    //Remake edge with new from and to
    edge = to*N + from;
    
    //Add edge to edge list.
    percolationHistory.push_back(edge);
    
    //Add nodes to each other's neighbor lists
    netNodes[from]->addNeighbor(netNodes[to]);
    netNodes[to]->addNeighbor(netNodes[from]);
    //Push the GC into the GC vector
    timeSeriesGC.push_back(GC);
    
    // Send the signal that things have changed.
    emit sendDataPoint(float(timeSeriesGC.size())/N,float(GC)/N);
}
/*
 * gets edges for the percolation process according to different rules
 */
long int Percolation :: getEdge(string rule, int N, vector<Percnode*> *netNodes, long int *loser)
{
    long int edge=0;
    bool isValidEdge=0;
    long int from=0;
    long int to=0;
    
    //Achlioptas PR
    if (rule.compare("PR")==0)
    {
        long int too;
        long int fromm;
        while(!isValidEdge)
        {
            //Pick 4 random nodes.
            from = random() % N;
            fromm = random() % N;
            to = random() % N;
            too = random () % N;
            
            //innocent until proven guilty...isValidEdge = 1
            isValidEdge=1;
            
            //All 4 nodes must be distinct from each other
            if (from == to || from == too || from == fromm || 
                to == too || to == fromm || too == fromm)
            {
                isValidEdge=0;
            }
            
            //from-to and fromm-too must not already be connected.
            if (isValidEdge)
            {
                //from-to
                for (vector<Percnode*>::iterator oldNeighbor = (*netNodes)[from]->getNeighbors()->begin();
                     oldNeighbor != (*netNodes)[from]->getNeighbors()->end();
                     ++oldNeighbor)
                {
                    
                    if ((*oldNeighbor)->getID() == (*netNodes)[to]->getID())
                    {
                        isValidEdge=0;
                        break;
                    }
                }
                //fromm-too
                for (vector<Percnode*>::iterator oldNeighbor = (*netNodes)[fromm]->getNeighbors()->begin();
                     oldNeighbor != (*netNodes)[fromm]->getNeighbors()->end();
                     ++oldNeighbor)
                {
                    if ((*oldNeighbor)->getID() == (*netNodes)[too]->getID())
                    {
                        isValidEdge=0;
                        break;
                    }
                }
            }
        }
        //Now that we know that both edges COULD be ok, we choose which one to make.
        //if to*from is smaller, make from-to. if too*fromm is smaller, from-too. if equal
        //then arbitrarily pick from-to	
        if ( 
            (*netNodes)[to]->getCluster()->getSize() * (*netNodes)[from]->getCluster()->getSize() 
            <= 
            (*netNodes)[too]->getCluster()->getSize() * (*netNodes)[fromm]->getCluster()->getSize() )
        {
            edge = (to * N) + from;
            (*loser) = (too * N) + fromm;
        }
        else
        {
            edge = (too * N) + fromm;
            (*loser) = (to * N) + from;
        }
    }
    
    //Adjacent Edge PR
    if (rule.compare("AE")==0)
    {
        long int too;
        while(!isValidEdge)
        {
            //Pick 3 random nodes.
            from = random() % N;
            to = random() % N;
            too = random () % N;
            
            //innocent until proven guilty...isValidEdge = 1
            isValidEdge=1;
            
            //All 3 nodes must be distinct from each other
            if (from == to || from == too || to==too)
            {
                isValidEdge=0;
            }
            
            //from-to and from-too must not already be connected.
            if (isValidEdge)
            {
                for (vector<Percnode*>::iterator oldNeighbor = (*netNodes)[from]->getNeighbors()->begin();
                     oldNeighbor != (*netNodes)[from]->getNeighbors()->end();
                     ++oldNeighbor)
                {
                    //from-to
                    if ((*oldNeighbor)->getID() == (*netNodes)[to]->getID())
                    {
                        isValidEdge=0;
                        break;
                    }
                    //from-too
                    if ((*oldNeighbor)->getID() == (*netNodes)[too]->getID())
                    {
                        isValidEdge=0;
                        break;
                    }
                }
            }
        }
        //Now that we know that both edges COULD be ok, we choose which one to make.
        //if to's cluster is smaller, make that from-to. if too's, make from-too. if equal
        //then arbitrarily pick from-to	
        if ( (*netNodes)[to]->getCluster()->getSize() <= (*netNodes)[too]->getCluster()->getSize() )
        {
            edge = (to * N) + from;
        }
        else
        {
            edge = (too * N) + from;
        }
    }
    
    //Erdos Renyi
    if (rule.compare("ER")==0)
    {
        //Generate an edge to make
        
        while(!isValidEdge)
        {
            from = random() % N;
            to = random() % N;
            
            //innocent until proven guilty...isValidEdge = 1
            isValidEdge=1;
            
            //from and to must be different. If they are the same, not isValidEdge.
            if (from == to)
            {
                isValidEdge=0;
            }
            //nodes must not already be connected. If we think the edge is still valid, check
            if (isValidEdge)
            {
                for (vector<Percnode*>::iterator oldNeighbor = (*netNodes)[from]->getNeighbors()->begin();
                     oldNeighbor != (*netNodes)[from]->getNeighbors()->end();
                     ++oldNeighbor)
                {
                    if ((*oldNeighbor)->getID() == (*netNodes)[to]->getID())
                    {
                        isValidEdge=0;
                        break;
                    }			
                }
            }
        }
        edge = (to*N)+from;
    }
    
    return edge;
}
/********************************************************************/
/************************** SLOTS ***********************************/
/********************************************************************/
/*
 * Jiggles the nodes
 */
void Percolation :: jiggle()
{
    float jiggledX;
    float jiggledY;
    for(unsigned int i=0; i < netNodes.size(); ++i)
    {
        do
        {
            jiggledX = netNodes[i]->getX()+0.02*(2.0*random()/RAND_MAX-1.0);
            jiggledY = netNodes[i]->getY()+0.02*(2.0*random()/RAND_MAX-1.0);
        }
        while(jiggledX > 1
              || jiggledX < 0
              || jiggledY > 1
              || jiggledY < 0);
        netNodes[i]->setX(jiggledX);
        netNodes[i]->setY(jiggledY);
    }
    updateGL();
}
/*
 * Jiggles the nodes
 */
void Percolation :: addLinks()
{
    for(int i=0; i < numberLinksAtOnce; ++i)
    {
        if(percolationHistory.size() < 5*N)
        {
            percolate();
        }
    }
    updateGL();
}
/*
 * Set the number of links to be added at once
 */
void Percolation :: setNumberLinksAtOnce(int index)
{
    if(index == 0)
    {
        numberLinksAtOnce = 1;
    }
    else if(index == 1)
    {
        numberLinksAtOnce = 2;
    }
    else if(index == 2)
    {
        numberLinksAtOnce = 3;
    }
    else if(index == 3)
    {
        numberLinksAtOnce = 4;
    }
    else if(index == 4)
    {
        numberLinksAtOnce = 5;
    }
    else if(index == 5)
    {
        numberLinksAtOnce = 10;
    }
    else if(index == 6)
    {
        numberLinksAtOnce = 25;
    }
}
/*
 * Set the thickness of the fluid
 */
void Percolation :: setFluidThickness(int index)
{
    if(index==0)
    {
        coeffDrag = 1.0;
    }
    else if(index==1)
    {
        coeffDrag = 2.5;        
    }
    else if(index==2)
    {
        coeffDrag = 5.0;
    }
    else if(index==3)
    {
        coeffDrag = 10.0;
    }
    else if(index==4)
    {
        coeffDrag = 20.0;
    }
    
}
/*
 * Set the percolation Rule
 */
void Percolation :: setPercolationRule(int index)
{
    if(index==0)
    {
        rule = "ER";
    }
    else if(index == 1)
    {
        rule = "AE";
    }
    else if(index == 2)
    {
        rule = "PR";
    }
}
/*
 * Toggle the SC bool
 */
void Percolation :: toggleSC(int signal)
{
    isSC = signal;
}
/*
 * Toggle the showInfo bool
 */
void Percolation :: toggleShowInfo(int signal)
{
    showInfo = signal;
}
/*
 * Change the SC Value
 */
void Percolation :: changeSCValue(int signal)
{
    SC = signal+1;
}
/*
 * Toggle the showSCBrain bool
 */
void Percolation :: toggleSCBrain(int signal)
{
    showSocialClimberBrain = signal;
}
/*
 * Toggle the showPRBrain bool
 */
void Percolation :: togglePRBrain(int signal)
{
    showProductRuleBrain = signal;
}
/*
 * Change radius value
 */
void Percolation :: changeRadiusValue(int signal)
{
    radius = signal;
}
/*
 * Change newPercolationNodes
 */
void Percolation :: changeN(int signal)
{
    switch (signal)
    {
        case 0:
            newPercolationNodes = 50;
            break;
        case 1:
            newPercolationNodes = 100;
            break;
        case 2:
            newPercolationNodes = 150;
            break;
        case 3:
            newPercolationNodes = 250;
            break;
        case 4:
            newPercolationNodes = 500;
            break;
    }
}
/*
 * Toggle Splash on/off (at start)
 */
void Percolation :: disableSplash()
{
    showSplash = false;
}


