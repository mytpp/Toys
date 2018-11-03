#include "mainwindow.h"
#include <QApplication>
#include "Forum/forum.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Forum::SetExistUsers();

    MainWindow w;

    return a.exec();
}
