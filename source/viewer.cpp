//
//  Hello Widget
//

#include <QtGui>
#include "viewer.h"
#include "Percolation.h"
#include "Graph.h"
#include <QTimer>

//
//  Constructor for Viewer widget
//
Viewer::Viewer(QWidget* parent)
:  QWidget(parent)
{
	//  Set window title
	setWindowTitle(tr("Percolation Visualizer"));

    //  Create new percolation
	Percolation* percolation = new Percolation;
	//  Set up the new percolation
	percolation->setupNewPercolation();
    
    //  Create new graph
    Graph* graph = new Graph;
    //  Set up the new graph
    graph->setupNewGraph();
    
    //  Checkbox to toggle display info
    QCheckBox* showInfoCheck    = new QCheckBox("Display info.");
    showInfoCheck->setCheckState(Qt::Checked);
    //  Checkbox to toggle social climber
    QCheckBox* SCCheck          = new QCheckBox("Social climber");
    SCCheck->setCheckState(Qt::Unchecked);
    //  Checkbox to toggle showing social climber brain
    QCheckBox* showSCBrainCheck = new QCheckBox("Show proposed link");
    showSCBrainCheck->setCheckState(Qt::Unchecked);
    showSCBrainCheck->setVisible(false);
    //  Checkbox to toggle showing social climber brain
    showPRBrainCheck = new QCheckBox("Show PR link");
    showPRBrainCheck->setCheckState(Qt::Unchecked);
    showPRBrainCheck->setVisible(false);
    
	//  Pushbutton to add Links
	QPushButton* addLinksButton	= new QPushButton("Add Link(s)");
	//  Pushbutton to jiggle display
	QPushButton* jiggleButton	= new QPushButton("Jiggle Display");
    //  Pushbutton to restart percolation
    resetButton	= new QPushButton("New Percolation");
    //  Pushbutton to restart percolation
    QPushButton* quitButton     = new QPushButton("Quit");
	
	//  Combo box for number of links to add at a time
	QComboBox* numberLinksAtOnceBox = new QComboBox;
	numberLinksAtOnceBox->addItems(QStringList()
								   << "1 at a time"
								   << "2 at a time"
								   << "3 at a time"
								   << "4 at a time"
								   << "5 at a time"
								   << "10 at a time"
								   << "25 at a time");
	QComboBox* fluidThicknessBox = new QComboBox;
	fluidThicknessBox->addItems(QStringList()	
								   << "Very Thin"
								   << "Thin"
								   << "Moderate"
								   << "Thick"
								   << "Very Thick");
    fluidThicknessBox->setCurrentIndex(2);
	QComboBox* percolationRuleBox   = new QComboBox;
	percolationRuleBox->addItems(QStringList()	
                                    << "Erdos-Renyi (Random)"
                                    << "Adjacent Edge"
                                    << "Product Rule");
    QComboBox* SCBox     = new QComboBox;
    SCBox->addItems(QStringList()
                                    << "Only one side"
                                    << "Both sides");
    SCBox->setVisible(false);
    QComboBox* radiusBox = new QComboBox;
    radiusBox->addItems(QStringList()
                                    << "Infinite"
                                    << "1"
                                    << "2"
                                    << "3"
                                    << "4"
                                    << "5");
    radiusBox->setVisible(false);
    nBox = new QComboBox;
    nBox->addItems(QStringList()
                                    << "with 50 nodes"
                                    << "with 100 nodes"
                                    << "with 150 nodes"
                                    << "with 250 nodes"
                                    << "with 500 nodes");
    nBox->setCurrentIndex(2);
    
    // Declare a label here so we can change its visibility
    QLabel* radiusLabel = new QLabel("Search radius");
    radiusLabel->setVisible(false);
    QLabel* sidedLabel = new QLabel("Applied to");
    sidedLabel->setVisible(false);
    
	//  Connect buttons to slots
    connect(nBox                    , SIGNAL(currentIndexChanged(int))  , percolation       , SLOT(changeN(int)));
    connect(percolation             , SIGNAL(resetGraph())              , graph             , SLOT(reset()));
    connect(percolation             , SIGNAL(sendDataPoint(float,float)), graph             , SLOT(receiveDataPoint(float,float)));
    connect(radiusBox               , SIGNAL(currentIndexChanged(int))  , percolation       , SLOT(changeRadiusValue(int)));
    connect(showSCBrainCheck        , SIGNAL(stateChanged(int))         , percolation       , SLOT(toggleSCBrain(int)));
    connect(showPRBrainCheck        , SIGNAL(stateChanged(int))         , percolation       , SLOT(togglePRBrain(int)));
    connect(SCBox                   , SIGNAL(currentIndexChanged(int))  , percolation       , SLOT(changeSCValue(int)));
    connect(SCCheck                 , SIGNAL(stateChanged(int))         , percolation       , SLOT(toggleSC(int)));
    connect(SCCheck                 , SIGNAL(toggled(bool))             , sidedLabel        , SLOT(setVisible(bool)));
    connect(SCCheck                 , SIGNAL(toggled(bool))             , radiusLabel       , SLOT(setVisible(bool)));
    connect(SCCheck                 , SIGNAL(toggled(bool))             , radiusBox         , SLOT(setVisible(bool)));
    connect(SCCheck                 , SIGNAL(toggled(bool))             , showSCBrainCheck  , SLOT(setVisible(bool)));
    connect(SCCheck                 , SIGNAL(toggled(bool))             , SCBox             , SLOT(setVisible(bool)));
    connect(showInfoCheck           , SIGNAL(stateChanged(int))         , percolation       , SLOT(toggleShowInfo(int)));
    connect(quitButton              , SIGNAL(clicked(void))             , this              , SLOT(close()));
    connect(resetButton             , SIGNAL(clicked(void))             , percolation       , SLOT(reset()));
    connect(jiggleButton			, SIGNAL(clicked(void))             , percolation       , SLOT(jiggle()));
	connect(addLinksButton			, SIGNAL(clicked(void))             , percolation       , SLOT(addLinks()));
	connect(numberLinksAtOnceBox	, SIGNAL(currentIndexChanged(int))  , percolation       , SLOT(setNumberLinksAtOnce(int)));
	connect(fluidThicknessBox		, SIGNAL(currentIndexChanged(int))	, percolation       , SLOT(setFluidThickness(int)));
	connect(percolationRuleBox		, SIGNAL(currentIndexChanged(int))	, percolation       , SLOT(setPercolationRule(int)));
	connect(percolationRuleBox      , SIGNAL(currentIndexChanged(int))  , this              , SLOT(togglePRCheck(int)));
    
	//  Set layout of child widgets
	QGridLayout* layout = new QGridLayout;
	layout->setColumnStretch(0,100);
    layout->setColumnStretch(1,10);
	layout->setColumnMinimumWidth(0,500);
	layout->setRowStretch(3,100);
    layout->setRowMinimumHeight(3,200);
	
	//  Percolation widget
	layout->addWidget(percolation,0,0,5,1);
	
	//  Group Percolation parameters
	percolationGroupBox = new QGroupBox("Percolation Parameters");
	QGridLayout* percolationGridLayout = new QGridLayout;
	percolationGridLayout->addWidget(new QLabel("Percolation rule"),0,0);
	percolationGridLayout->addWidget(percolationRuleBox,0,1);
    percolationGridLayout->addWidget(showPRBrainCheck,1,0);
    percolationGridLayout->addWidget(SCCheck,2,0);
    percolationGridLayout->addWidget(sidedLabel,3,0);
    percolationGridLayout->addWidget(SCBox,3,1);
    percolationGridLayout->addWidget(radiusLabel,4,0);
    percolationGridLayout->addWidget(radiusBox,4,1);
    percolationGridLayout->addWidget(showSCBrainCheck,2,1);
	percolationGridLayout->addWidget(new QLabel("# Links to add"),5,0);      
	percolationGridLayout->addWidget(numberLinksAtOnceBox,5,1);
	percolationGridLayout->addWidget(addLinksButton,6,0,1,2);
	percolationGroupBox->setLayout(percolationGridLayout);
	layout->addWidget(percolationGroupBox,0,1);
    percolationGroupBox->setEnabled(false);
	
	//  Group Display parameters
	displayGroupBox = new QGroupBox("Display");
	QGridLayout* displayGridLayout = new QGridLayout;
	displayGridLayout->addWidget(new QLabel("Fluid thickness"),0,0);  
	displayGridLayout->addWidget(fluidThicknessBox,0,1);
	displayGridLayout->addWidget(jiggleButton,1,1);
    displayGridLayout->addWidget(showInfoCheck,1,0);
	displayGroupBox->setLayout(displayGridLayout);
	layout->addWidget(displayGroupBox,1,1);
    displayGroupBox->setEnabled(false);
    
    //  Group Command parameters
    QGroupBox* commandGroupBox = new QGroupBox("Master");
    QGridLayout* commandGridLayout = new QGridLayout;
    commandGridLayout->addWidget(resetButton,0,0);
    commandGridLayout->addWidget(nBox,0,1);
    commandGridLayout->addWidget(quitButton,1,0,1,2);
    commandGroupBox->setLayout(commandGridLayout);
    layout->addWidget(commandGroupBox,2,1);
    resetButton->setEnabled(false);
    nBox->setEnabled(false);
	
    //  Graph
    QGroupBox* graphGroupBox = new QGroupBox("GC vs. Links Added");
    QGridLayout* graphGridLayout = new QGridLayout;
    graphGridLayout->addWidget(graph,0,0);
    graphGroupBox->setLayout(graphGridLayout);
    layout->addWidget(graphGroupBox,3,1,-1,-1);

	//  Overall layout
	setLayout(layout);

    
    
    //  Timer
    QTimer *splashTimer = new QTimer(this);
    QTimer *timer = new QTimer(this);
    timer->setInterval(5);
    splashTimer->setSingleShot(true);
    connect(splashTimer,    SIGNAL(timeout()),          percolation,    SLOT(disableSplash()));
    connect(splashTimer,    SIGNAL(timeout()),          percolation,    SLOT(reset()));
    connect(splashTimer,    SIGNAL(timeout()),          timer,          SLOT(start()));
    connect(splashTimer,    SIGNAL(timeout()),          this,           SLOT(unSplash()));
    connect(timer,          SIGNAL(timeout()),          percolation,    SLOT(updateXY()));
    connect(timer,          SIGNAL(timeout()),          graph,          SLOT(updateGL()));
    
    splashTimer->start(3000);
}
/*
 * Unsplash the menus
 */
void Viewer :: unSplash()
{
    nBox->setEnabled(true);
    resetButton->setEnabled(true);
    displayGroupBox->setEnabled(true);
    percolationGroupBox->setEnabled(true);
}
/*
 * Show/Hide the PRBrainCheck box
 */
void Viewer :: togglePRCheck(int signal)
{
    if(signal == 2)
    {
        showPRBrainCheck->setVisible(true);
    }
    else if(signal < 2)
    {
        showPRBrainCheck->setVisible(false);
    }
}