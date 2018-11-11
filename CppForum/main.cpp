#include "mainwindow.h"
#include "UI/logindialog.h"
#include <QApplication>
#include "Forum/forum.h"

namespace ui {

LoginDialog *loginDialog;
MainWindow *mainWindow;

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Forum::SetExistUsers();

    ui::loginDialog = new LoginDialog;
    ui::loginDialog->show();

    //the main window needs to be on heap,
    //so that we can delete it when user logs out
    ui::mainWindow = new MainWindow;


    return a.exec();
}
