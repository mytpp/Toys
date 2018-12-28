#include <QCoreApplication>
#include <QSqlDatabase>
#include "Storage/forumstorage.h"
#include "Forum/forum.h"
#include "forumserver.h"
#include <QDebug>

static constexpr bool first_initiate = false;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase(QObject::tr("QSQLITE"));
    db.setHostName(QObject::tr("myhost"));
    db.setDatabaseName(QObject::tr("forum.db"));
    db.setUserName(QObject::tr("mytpp"));
    db.setPassword(QObject::tr("mytpp"));
    db.open();
    if(first_initiate)
        qInfo()<<"InitiateStorage: "
               <<ForumStorage::InitiateStorage();

    Forum::SetExistUsers();

    ForumServer server(9999);

    return a.exec();
}
