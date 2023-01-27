#include "mainwindow.h"
#include "threads.h"

#include <QApplication>
#include <gst/gst.h>

    MainWindow *w;
    threads *mthread;
//    s_test_task enumValues;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    qRegisterMetaType<QList>("QString");


    w = new MainWindow();
    w->show();
    return a.exec();


}
