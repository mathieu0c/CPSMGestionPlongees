#include "GUI/MainWindow.h"

#include <QApplication>

//#include "DataStruct/global.cpp"
//#include "GUI/global.cpp"
//#include "DBApi/global.cpp"
//#include "Debug/global.cpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gui::MainWindow w;
    w.show();
    return a.exec();
}
