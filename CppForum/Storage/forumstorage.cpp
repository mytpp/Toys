#include "forumstorage.h"
#include "Storage/userinfostorage.h"
#include "Storage/boardsstorage.h"
#include "Storage/postsstorage.h"
#include "Storage/commentsstorage.h"
#include <QSqlQuery>
#include <QException>
#include <QDebug>

ForumStorage::ForumStorage()
{

}

bool ForumStorage::InitiateStorage() {
    QSqlQuery query;
    //create userinfo table
    bool success = query.exec(
               "create table if not exists userinfo ("
               "id         varchar primary key,"
               "status     char(20),"
               "postsCount int,"
               "name       varchar,"
               "password   varchar)"
            );
    if(success) {
        qDebug()<<"create userinfo table successfully";
    } else{
        qDebug()<<"userinfo table failed";
        return false;
    }

    //create boards table
    success = query.exec(
                "create table if not exists boards ("
                "name      varchar,"
                "moderator varchar)"
             );
    if(success) {
        qDebug()<<"create boards table successfully";
    } else{
        qDebug()<<"boards table failed";
        return false;
    }

    //create posts table
    success = query.exec(
                "create table if not exists posts ("
                "id       varchar primary key,"
                "poster   varchar,"
                "authorId varchar,"
                "title    varchar,"
                "content  varchar,"
                "birthday varchar)"
             );
    if(success) {
        qDebug()<<"create posts table successfully";
    } else{
        qDebug()<<"posts table failed";
        return false;
    }

    //create comments table
    success = query.exec(
                "create table if not exists comments ("
                "authorId varchar,"
                "content  varchar,"
                "birthday varchar)"
             );
    if(success) {
        qDebug()<<"create comments table successfully";
    } else{
        qDebug()<<"comments table failed";
        return false;
    }

    return true;
}

ForumStorage& ForumStorage::GetNullValue() {
    static NullStorage     null;
    return null;
}

ForumStorage& ForumStorage::GetStorage(const QString &category) {
    static UserInfoStorage userInfoStorage;
    static BoardsStorage   boardsStorage;
    static PostsStorage    postsStorage;
    static CommentsStorage commentsStorage;


    if(category == "userinfo") {
        return userInfoStorage;
    } else if (category == "boards") {
        return boardsStorage;
    } else if (category == "posts") {
        return postsStorage;
    } else if (category == "comments") {
        return commentsStorage;
    } else {
        qDebug()<<"Storage Not Found";
        return GetNullValue();
    }
}

ForumStorage& NullStorage::operator <<(QVector<QString>&) {
    throw QException();
}

ForumStorage& NullStorage::operator >>(QVector<QString>&) {
    throw QException();
}
