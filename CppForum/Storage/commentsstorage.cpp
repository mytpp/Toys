#include "commentsstorage.h"
#include <QDebug>

CommentsStorage::CommentsStorage()
{

}

ForumStorage& CommentsStorage::operator <<(QVector<QString>& record) {
    if(record.size() != 3){
        qDebug()<<"A comment record must have 3 fields";
        return ForumStorage::GetNullValue();
    }

    query.prepare("insert into comments values (?,?,?,?,?)");
    query.addBindValue(record[0]);
    query.addBindValue(record[1]);
    query.addBindValue(record[2]);
    query.addBindValue(record[3]);
    query.addBindValue(record[4]);

    bool success = query.exec();
    if(success) {
        qDebug()<<"insert into comments table successfully";
    } else {
        qDebug()<<"inserting into comments table failed";
        return ForumStorage::GetNullValue();
    }

    return *this;
}

ForumStorage& CommentsStorage::operator >>(QVector<QString>& record) {
    return ForumStorage::GetNullValue();
}
