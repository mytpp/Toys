#include "postsstorage.h"
#include <QDebug>

PostsStorage::PostsStorage()
{

}

ForumStorage& PostsStorage::operator <<(QVector<QString>& record) {
    if(record.size() != 7){
        qDebug()<<"A post record must have 7 fields";
        return ForumStorage::GetNullValue();
    }

    query.prepare("insert into posts values (?,?,?,?,?,?,?)");
    query.addBindValue(record[0]);
    query.addBindValue(record[1]);
    query.addBindValue(record[2]);
    query.addBindValue(record[3]);
    query.addBindValue(record[4]);
    query.addBindValue(record[5]);
    query.addBindValue(record[6]);

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
    if(!dataAvailable){
        dataAvailable = query.exec(
            "select id, board, author, authorId, title, content, birthday from posts"
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

    return *this;
}
