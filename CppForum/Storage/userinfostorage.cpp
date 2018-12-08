#include "userinfostorage.h"
#include <QDebug>

UserInfoStorage::UserInfoStorage()
{

}

ForumStorage& UserInfoStorage::operator <<(QVector<QString>& record) {
    if(record.size() != 5){
        qDebug()<<"A userinfo record must have 5 fields";
        return ForumStorage::GetNullValue();
    }

    query.prepare("insert into userinfo values (?,?,?,?,?)");
    query.addBindValue(record[0]);
    query.addBindValue(record[1]);
    query.addBindValue(record[2].toInt());
    query.addBindValue(record[3]);
    query.addBindValue(record[4]);

    bool success = query.exec();
    if(success) {
        qDebug()<<"insert into userinfo table successfully";
    } else {
        qDebug()<<"inserting into userinfo table failed";
        return ForumStorage::GetNullValue();
    }

    return *this;
}

ForumStorage& UserInfoStorage::operator >>(QVector<QString>& record) {
    //We don't use "select *" here,
    //since the order of the columns is not defined if "select *" is used.
    if(!dataAvailable){
        dataAvailable = query.exec(
            "select id, status, postsCount, name, password from userinfo"
        );
        //qDebug()<<"query size: "<<query.numRowsAffected(); -> 0
        query.next();
    }

    if(query.isValid()) {
        dataAvailable = true;
        for(int i=0; i<5; i++)
            record.push_back(query.value(i).toString());
        query.next();  // so that we can see if there is remaining record in the
                       // result of 'select statement' (via query.isValid())
    } else {
        dataAvailable = false;
    }

    return *this;
}

