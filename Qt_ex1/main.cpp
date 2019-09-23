//
//  Qt OpenGL Hello World
//  Adapted from Trolltech hellogl example
//

#include <QApplication>
#include "hello.h"

//
//  Main function
//
int main(int argc, char *argv[])
{
   //  Create the application
   QApplication app(argc,argv);
   //  Create and show Hello widget
   Hello hello;
   hello.show();
   //  Main loop for application
   return app.exec();
}
