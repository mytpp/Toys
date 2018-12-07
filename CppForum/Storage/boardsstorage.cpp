#include "boardsstorage.h"
#include <QDebug>

BoardsStorage::BoardsStorage()
{

}

ForumStorage& BoardsStorage::operator <<(QVector<QString>& record) {
    if(record.size() != 2){
        qDebug()<<"A board record must have 2 fields";
        return ForumStorage::GetNullValue();
    }

    query.prepare("insert into boards values (?,?)");
    query.addBindValue(record[0]);
    query.addBindValue(record[1]);

    bool success = query.exec();
    if(success) {
        qDebug()<<"insert into boards table successfully";
    } else {
        qDebug()<<"inserting into boards table failed";
        return ForumStorage::GetNullValue();
    }

    return *this;
}

ForumStorage& BoardsStorage::operator >>(QVector<QString>& record) {
    return ForumStorage::GetNullValue();
}
