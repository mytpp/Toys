#include "mainwindow.h"
#include "UI/logindialog.h"
#include <QApplication>
#include "Forum/forum.h"
#include <QMessageBox>
#include <QDebug>

namespace ui {
//definition
std::unique_ptr<LoginDialog> loginDialog;
std::unique_ptr<MainWindow> mainWindow;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ui::loginDialog.reset(new LoginDialog);
    ui::loginDialog->show();

    return a.exec();
}
