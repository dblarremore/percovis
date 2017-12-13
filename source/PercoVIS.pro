#  Project file for the lorenz viewer
#  Dan Larremore
#

TARGET = PercoVIS

TEMPLATE = app

# icons
ICON = PercoVIS.icns

#  List of header files
HEADERS = viewer.h Percolation.h Percnode.h Cluster.h Graph.h

#  List of source files
SOURCES = main.cpp viewer.cpp Percolation.cpp Percnode.cpp Cluster.cpp Graph.cpp

#  Include QT support
CONFIG += release

#  Include OpenGL support
QT += core
QT += gui
QT += opengl