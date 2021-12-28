#include "GUI/MainWindow.h"

#include <QApplication>

//define global var
//#include "Info/global.cpp"
//#include "GUI/global.cpp"
//#include "Data/global.cpp"
//#include "Debug/global.cpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gui::MainWindow w;
    w.show();
    return a.exec();
}
