#include "commentsstorage.h"
#include <QDebug>

CommentsStorage::CommentsStorage()
{
    query.exec("select * from comments");
    query.last();
    nextId = query.at() + 1;
}

ForumStorage& CommentsStorage::operator <<(QVector<QString>& record) {
    if(record.size() != 7){
        qDebug()<<"A comment record must have 7 fields";
        return ForumStorage::GetNullValue();
    }

    query.prepare("insert into comments values (?,?,?,?,?,?,?)");
    query.addBindValue(record[0]);
    query.addBindValue(record[1]);
    query.addBindValue(record[2]);
    query.addBindValue(record[3]);
    query.addBindValue(record[4]);
    query.addBindValue(record[5]);
    query.addBindValue(record[6]);

    bool success = query.exec();
    if(success) {
        qDebug()<<"insert into comments table successfully";
    } else {
        qDebug()<<"inserting into comments table failed";
        return ForumStorage::GetNullValue();
    }

    lastOpration = IN;
    return *this;
}

ForumStorage& CommentsStorage::operator >>(QVector<QString>& record) {
    if(!dataAvailable){
        dataAvailable = query.exec(
            "select id, board, postid, author, authorId, content, birthday from comments"
        );
        query.next();
    }

    if(query.isValid()) {
        dataAvailable = true;
        for(int i=0; i<7; i++)
            record.push_back(query.value(i).toString());
        query.next();  // so that we can see if there is remaining record in the
                       // result of 'select statement' (via query.isValid())
    } else {
        dataAvailable = false;
    }

    lastOpration = OUT;
    return *this;
}

