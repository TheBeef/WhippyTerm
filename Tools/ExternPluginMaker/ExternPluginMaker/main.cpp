#include "ExternPluginMaker.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ExternPluginMaker w;
    w.show();
    
    return a.exec();
}
