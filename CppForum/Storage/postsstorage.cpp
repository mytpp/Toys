#include "postsstorage.h"
#include <QDebug>

PostsStorage::PostsStorage()
{

}

ForumStorage& PostsStorage::operator <<(QVector<QString>& record) {
    if(record.size() != 6){
        qDebug()<<"A post record must have 6 fields";
        return ForumStorage::GetNullValue();
    }

    query.prepare("insert into posts values (?,?,?,?,?,?)");
    query.addBindValue(record[0]);
    query.addBindValue(record[1]);
    query.addBindValue(record[2]);
    query.addBindValue(record[3]);
    query.addBindValue(record[4]);
    query.addBindValue(record[5]);

    bool success = query.exec();
    if(success) {
        qDebug()<<"insert into posts table successfully";
    } else {
        qDebug()<<"inserting into posts table failed";
        return ForumStorage::GetNullValue();
    }

    return *this;
}

ForumStorage& PostsStorage::operator >>(QVector<QString>& record) {
    return ForumStorage::GetNullValue();
}
