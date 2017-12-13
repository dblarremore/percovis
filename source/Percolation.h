#ifndef PERCOLATION_H
#define PERCOLATION_H

#include <QGLWidget>
#include <QString>
#include "Percnode.h"
#include "Cluster.h"

class Percolation : public QGLWidget
{
	Q_OBJECT
private:
	float PI;								// PI
	float nodeRadius;						// Size of Nodes on screen
	float springLength;						// Resting length of springs
	float distances[3][1000][1000];			// Matrix [3][1000][1000] of distances
    
	float coeffDrag;						// Fluid drag coefficient
	float coeffElectrostatic;				// Electrostatic repulsion coefficient
	float coeffBoundaryStrength;			// Boundary strength coefficient
	float coeffSpring;						// Linear spring coefficient
    
	int totalColors;						// Number of cluster colors
	float colors [100];						// Array of colors
	
	unsigned int N;							// Number of nodes
    unsigned int newPercolationNodes;       // Number of nodes for a new percolation
	unsigned int T;							// Number of links added
    int numberLinksAtOnce;                  // Number of links to add each time;
	float timestep;							// Integration timestep
	string rule;							// Percolation rule
	
	vector<Percnode*> netNodes;				// Vector of Percnodes
	vector<long int> percolationHistory;	// History of links added
	vector<int> fromHistory;				// History of froms
	vector<int> toHistory;					// History of tos
	
	vector<double> ones;					// Ones use for matrixmaking
	
	long unsigned int edge;					// Edge
	long unsigned int from;					// From
	long unsigned int to;					// To
    long int PREdge;
	int GC;									// Size of Giant Component
	vector<unsigned int> timeSeriesGC;		// Time series of GC
	
    bool showInfo;                          // Toggle info shown
	bool showSocialClimberBrain;			// Toggle SC brain
    bool showProductRuleBrain;              // Toggle PR brain
    int fromPRBrain;                        // Alt PR from
    int toPRBrain;                          // Alt PR to
	int fromSCBrain;						// SC's original from
	int toSCBrain;							// SC's original to
	
	bool isPickyMeal;						// For SC cluster consumption
    bool isSC;                              // SC on or off
	unsigned int SC;						// Social Climber - {1,2}
	int radius;								// Social Climber radius
    
	bool mouse;								// Mouseclick
	QPoint pos;								// Mouse position 
    int grabbedNode;
    float mouseXGL;
    float mouseYGL;
    
    double dim;                             // Global dimension
    double aspectRatio;                     // Aspect Ratio
    int viewWidth;
    int viewHeight;
    int textSpacer;
    
    QFont aFont;
    QFont bFont;
    QFont splashFont;
    
    bool showSplash;
public:
	Percolation(QWidget* parent=0);
	QSize sizeHint() const {return QSize(800,800);}
    
    /*
     * Setup Functions
     */
    void fixColors();
    void setupNewPercolation();
    /*
     * Display Functions
     */
    void splash();
    void updateDistances();
    void printGC();
    void printTime();
    void printRule();
    void printSC();
    void drawLink(int,int);
    void drawCircle(float,float,float,int);
    void renderLinks();
    void renderSocialClimberLinks();
    void renderProductRuleLinks();
    void renderNodes();
    /*
     * Percolation Process Functions
     */
    void percolate();
    long int getEdge(string, int, vector<Percnode*>*, long int*);

public slots:
    void disableSplash();
    void changeN(int);
    void changeRadiusValue(int);
    void togglePRBrain(int);
    void toggleSCBrain(int);
    void changeSCValue(int);
    void toggleSC(int);
    void toggleShowInfo(int);
    void updateXY();
    void jiggle();
    void addLinks();
    void setNumberLinksAtOnce(int);
    void setFluidThickness(int);
    void setPercolationRule(int);
    void reset();
	
signals:
    void sendDataPoint(float,float);
    void resetGraph();
    
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int,int);
    void mousePressEvent(QMouseEvent*);    //  Mouse pressed
    void mouseReleaseEvent(QMouseEvent*);  //  Mouse released
    void mouseMoveEvent(QMouseEvent*);     //  Mouse moved
private:
    void project();

};

#endif
