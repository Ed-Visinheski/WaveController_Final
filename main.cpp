#include <iostream>
#include <QApplication>
#include "WaveController.h"



int main(int argc, char *argv[]) 
{
    QApplication app(argc, argv);
    CWaveController controller;
    
    return app.exec();
}


