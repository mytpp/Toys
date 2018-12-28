#include "userinfostorage.h"
#include <QDebug>

UserInfoStorage::UserInfoStorage()
{

}

ForumStorage& UserInfoStorage::operator <<(QVector<QString>& record) {
    if(record.size() != 5){
        qInfo()<<"A userinfo record must have 5 fields";
        return ForumStorage::GetNullValue();
    }

    query.prepare("insert into userinfo values (?,?,?,?,?)");
    query.addBindValue(record[0]);
    query.addBindValue(record[1].toInt());
    query.addBindValue(record[2].toInt());
    query.addBindValue(record[3]);
    query.addBindValue(record[4]);

    bool success = query.exec();
    if(success) {
        qInfo()<<"insert into userinfo table successfully";
    } else {
        qInfo()<<"inserting into userinfo table failed";
        return ForumStorage::GetNullValue();
    }

    lastOpration = IN;
    return *this;
}

ForumStorage& UserInfoStorage::operator >>(QVector<QString>& record) {
    //We don't use "select *" here,
    //since the order of the columns is not defined if "select *" is used.
    if(!dataAvailable){
        dataAvailable = query.exec(
            "select id, status, postsCount, name, password from userinfo"
        );
        //qInfo()<<"query size: "<<query.numRowsAffected(); -> 0
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

    lastOpration = OUT;
    return *this;
}

bool UserInfoStorage::UpdateRecord(
        const QString &id, int index, const QString &newVal) {
    switch (index) {
    case 1: query.prepare("update userinfo set status = ? where id = ?");
        break;
    case 2: query.prepare("update userinfo set postCount = ? where id = ?");
        break;
    case 3: query.prepare("update userinfo set name = ? where id = ?");
        break;
    case 4: query.prepare("update userinfo set password = ? where id = ?");
        break;
    default:
        break;
    }
    query.bindValue(0, newVal);
    query.bindValue(1, id);
    if(!query.exec()){
        qInfo()<<"update userinfo where id="<<id<<"failed";
        return false;
    }
    qInfo()<<"update userinfo storage successfully";
    return true;
}

bool UserInfoStorage::RemoveRecord(const QString &id) {
    query.prepare("delete from userinfo where id=?");
    query.addBindValue(id);
    if(!query.exec()){
        qInfo()<<"delete from userinfo where id="<<id<<"failed";
        return false;
    }
    qInfo()<<"delete from userinfo successfully";
    return true;
}

