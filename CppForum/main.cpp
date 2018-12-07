#include "mainwindow.h"
#include "UI/logindialog.h"
#include <QApplication>
#include "Forum/forum.h"
#include "Storage/forumstorage.h"
#include <QSqlDatabase>
#include <QDebug>

namespace ui {
//definition
LoginDialog *loginDialog;
MainWindow *mainWindow;
}

static constexpr bool first_initiate = true;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase(QObject::tr("QSQLITE"));
    db.setHostName(QObject::tr("myhost"));
    db.setDatabaseName(QObject::tr("forum.db"));
    db.setUserName(QObject::tr("mytpp"));
    db.setPassword(QObject::tr("mytpp"));
    qDebug()<<db.open();
    if(first_initiate)
        qDebug()<<ForumStorage::InitiateStorage();

    Forum::SetExistUsers();

    ui::loginDialog = new LoginDialog;
    ui::loginDialog->show();

    //the main window needs to be on heap,
    //so that we can delete it when user logs out
    ui::mainWindow = new MainWindow;


    return a.exec();
}
