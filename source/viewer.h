#ifndef VIWER_H
#define VIWER_H

#include <QWidget>
#include <QtGui>
#include <Percolation.h>
#include <Graph.h>

class Viewer : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
private:
    QGroupBox* percolationGroupBox;
    QGroupBox* displayGroupBox;
    QComboBox* nBox;
    QPushButton* resetButton;
    QCheckBox* showPRBrainCheck;
public:
    Viewer(QWidget* parent=0); //  Constructor

private slots:
    void unSplash();
        void togglePRCheck(int);
};

#endif
