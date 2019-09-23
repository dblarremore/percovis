//
//  Hello Widget
//

#include <QtGui>
#include "hello.h"
#include "triangle.h"

//
//  Constructor for Hello widget
//
Hello::Hello()
{
   //  Set window title
   setWindowTitle(tr("Hello World Qt OpenGL"));

   //  Create new Triangle widget
   Triangle* triangle = new Triangle;

   //  Create slider and set range to 0-360 (degrees)
   QSlider* slider = new QSlider(Qt::Horizontal);
   slider->setRange(0,360);
   //  Eye candy - set tick interval for display
   slider->setTickInterval(30);
   slider->setTickPosition(QSlider::TicksBelow);

   //  Create label to show angle
   QLabel* label = new QLabel;

   //  Create toggle
   QComboBox* combo = new QComboBox();
   combo->addItem("Pastel Colors");
   combo->addItem("Texture");

   //  Set layout of child widgets
   QVBoxLayout* layout = new QVBoxLayout;
   layout->addWidget(slider);
   layout->addWidget(triangle);
   layout->addWidget(label);
   layout->addWidget(combo);
   setLayout(layout);

   //  Connect valueChanged() signal of slider to setTheta slot of triangle
   connect(slider, SIGNAL(valueChanged(int)) , triangle , SLOT(setTheta(int)));
   //  Connect toggled() signal of pushbutton to setTexture slot of triangle
   connect(combo, SIGNAL(currentIndexChanged(QString)) , triangle , SLOT(setType(QString)));
   //  Connect message() signal of triangle to setText slot of label
   connect(triangle, SIGNAL(message(QString)) , label , SLOT(setText(QString)));
}
