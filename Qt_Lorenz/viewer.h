#ifndef VIWER_H
#define VIWER_H

#include <QWidget>
#include <QDoubleSpinBox>

class Viewer : public QWidget
{
Q_OBJECT                 //  Qt magic macro
private:
   QDoubleSpinBox* s;    //  Spinbox for s
   QDoubleSpinBox* b;    //  Spinbox for b
   QDoubleSpinBox* r;    //  Spinbox for r
   QDoubleSpinBox* dt;   //  Spinbox for dt
   QDoubleSpinBox* dim;  //  Spinbox for dim
public:
   Viewer(QWidget* parent=0); //  Constructor
private slots:
   void setPAR(const QString&); //  Slot to set s,b,r,dt,dim
};

#endif
