#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    // https://forum.qt.io/topic/98417/how-to-create-singleinstance-application/3
    QSharedMemory _singular("myAppNameInstance");
    if(_singular.attach(QSharedMemory::ReadOnly)){
        _singular.detach();
        return INSTANCE_ALREADY_RUNNING;
    }else{
        _singular.create(1);
    }

    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    MainWindow w;
    w.show();
    return a.exec();
}
