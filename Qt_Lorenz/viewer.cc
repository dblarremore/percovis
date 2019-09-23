//
//  Hello Widget
//

#include <QtGui>
#include "viewer.h"
#include "lorenz.h"

//
//  Constructor for Viewer widget
//
Viewer::Viewer(QWidget* parent)
    :  QWidget(parent)
{
   //  Set window title
   setWindowTitle(tr("Qt OpenGL Lorenz Viewer"));

   //  Create new Lorenz widget
   Lorenz* lorenz = new Lorenz;

   //  Create spinboxes
   s   = new QDoubleSpinBox();
   b   = new QDoubleSpinBox();
   r   = new QDoubleSpinBox();
   dt  = new QDoubleSpinBox();
   dim = new QDoubleSpinBox();
   QDoubleSpinBox* x0  = new QDoubleSpinBox();
   QDoubleSpinBox* y0  = new QDoubleSpinBox();
   QDoubleSpinBox* z0  = new QDoubleSpinBox();
   //  Set range and values
   s->setDecimals(1);   s->setSingleStep(1.0);      s->setRange(-10,50);          s->setValue(10);
   b->setDecimals(2);   b->setSingleStep(0.1);      b->setRange(0,50);            b->setValue(2.6);
   r->setDecimals(1);   r->setSingleStep(1.0);      r->setRange(0,500);           r->setValue(28);
   x0->setDecimals(1);  x0->setSingleStep(0.1);     x0->setRange(0.1,5.0);        x0->setValue(1.0);
   y0->setDecimals(1);  y0->setSingleStep(0.1);     y0->setRange(0.1,5.0);        y0->setValue(1.0);
   z0->setDecimals(1);  z0->setSingleStep(0.1);     z0->setRange(0.1,5.0);        z0->setValue(1.0);
   dt->setDecimals(4);  dt->setSingleStep(0.0001);  dt->setRange(0.0001,0.1000);  dt->setValue(0.001);
   dim->setDecimals(1); dim->setSingleStep(1);      dim->setRange(1,1000);        dim->setValue(50);

   //  Display widget for angles and dimension
   QLabel* angles = new QLabel();
   //  Pushbutton to reset view angle
   QPushButton* reset = new QPushButton("Reset");

   //  Combo box for preset s/b/r & dt & dim values
   QComboBox* preset = new QComboBox;
   preset->addItems(QStringList() << "Preset SBR, dt & dim"
      << "A:10.0 ,  2.666 ,  28.0 , 0.001 ,  50"
      << "B:10.0 ,  2.67  ,  15.0 , 0.001 ,  25"
      << "C:10.0 ,  2.67  ,  12.0 , 0.001 ,  25"
      << "D:10.0 ,  2.666 , 250.0 , 0.001 , 500"
      << "E:17.0 ,  9.0   ,  16.5 , 0.03  ,  50"
      << "F:10.0 ,  0.50  ,  20.0 , 0.001 ,  50"
      << "G:-1.7 ,  2.66  ,  50.0 , 0.003 ,  75"
      << "H:-1.0 ,  2.66  ,  50.0 , 0.003 ,  50"
      << "I:-1.0 , 12.00  , 200.0 , 0.001 , 150"
      << "J:-1.0 ,  0.067 ,   2.9 , 0.01  ,   2"
      << "K:-3.0 ,  2.66  ,  50.0 , 0.003 , 500");

   //  Connect valueChanged() signals to Lorenz slots
   connect(s     , SIGNAL(valueChanged(double)) , lorenz , SLOT(setS(double)));
   connect(b     , SIGNAL(valueChanged(double)) , lorenz , SLOT(setB(double)));
   connect(r     , SIGNAL(valueChanged(double)) , lorenz , SLOT(setR(double)));
   connect(x0    , SIGNAL(valueChanged(double)) , lorenz , SLOT(setX0(double)));
   connect(y0    , SIGNAL(valueChanged(double)) , lorenz , SLOT(setY0(double)));
   connect(z0    , SIGNAL(valueChanged(double)) , lorenz , SLOT(setZ0(double)));
   connect(dt    , SIGNAL(valueChanged(double)) , lorenz , SLOT(setDT(double)));
   connect(dim   , SIGNAL(valueChanged(double)) , lorenz , SLOT(setDIM(double)));
   connect(reset , SIGNAL(clicked(void))        , lorenz , SLOT(reset(void)));
   //  Connect lorenz signals to display widgets
   connect(lorenz , SIGNAL(angles(QString)) , angles , SLOT(setText(QString)));
   connect(lorenz , SIGNAL(dimen(double))   , dim    , SLOT(setValue(double)));
   //  Connect combo box to setPAR in myself
   connect(preset , SIGNAL(currentIndexChanged(const QString&)), this , SLOT(setPAR(const QString&)));

   //  Set layout of child widgets
   QGridLayout* layout = new QGridLayout;
   layout->setColumnStretch(0,100);
   layout->setColumnMinimumWidth(0,100);
   layout->setRowStretch(4,100);

   //  Lorenz widget
   layout->addWidget(lorenz,0,0,5,1);

   //  Group SBR parameters
   QGroupBox* sbrbox = new QGroupBox("Lorenz Parameters");
   QGridLayout* sbrlay = new QGridLayout;
   sbrlay->addWidget(new QLabel("s (Prandtl)"),0,0);   sbrlay->addWidget(s,0,1);
   sbrlay->addWidget(new QLabel("b (Beta)"),1,0);      sbrlay->addWidget(b,1,1);
   sbrlay->addWidget(new QLabel("r (Rayleigh)"),2,0);  sbrlay->addWidget(r,2,1);
   sbrbox->setLayout(sbrlay);
   layout->addWidget(sbrbox,0,1);

   //  Group Origin parameters
   QGroupBox* xyzbox = new QGroupBox("Origin");
   QGridLayout* xyzlay = new QGridLayout;
   xyzlay->addWidget(new QLabel("x0"),0,0);  xyzlay->addWidget(x0,0,1);
   xyzlay->addWidget(new QLabel("y0"),1,0);  xyzlay->addWidget(y0,1,1);
   xyzlay->addWidget(new QLabel("z0"),2,0);  xyzlay->addWidget(z0,2,1);
   xyzbox->setLayout(xyzlay);
   layout->addWidget(xyzbox,1,1);

   //  Group Display parameters
   QGroupBox* dspbox = new QGroupBox("Display");
   QGridLayout* dsplay = new QGridLayout;
   dsplay->addWidget(new QLabel("dt"),0,0);  dsplay->addWidget(dt,0,1);
   dsplay->addWidget(new QLabel("dim"),1,0); dsplay->addWidget(dim,1,1);
   dsplay->addWidget(angles,2,0);            dsplay->addWidget(reset,2,1);
   dspbox->setLayout(dsplay);
   layout->addWidget(dspbox,2,1);

   //  Preset Values
   layout->addWidget(preset,3,1);

   //  Overall layout
   setLayout(layout);
}

//
//  Set SBR, dt & dim in viewer
//
void Viewer::setPAR(const QString& str)
{
   QStringList par = str.mid(2).split(',');
   if (par.size()<5) return;
   s->setValue(par[0].toDouble());
   b->setValue(par[1].toDouble());
   r->setValue(par[2].toDouble());
   dt->setValue(par[3].toDouble());
   dim->setValue(par[4].toDouble());
}
