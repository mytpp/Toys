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

    return false;
}

bool ForumStorage::InitiateUserInfo() {
    QSqlQuery query;

    //create userinfo table
    bool success = query.exec(
               "create table if not exists userinfo ("
               "id         varchar primary key,"
               "status     varchar,"
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

    //insert six users totally
    query.prepare("insert into userinfo values (?,?,?,?,?)");

    query.bindValue(0, "000");
    query.bindValue(1, "ADMINISTRATOR");
    query.bindValue(2, 0);
    query.bindValue(3, "a_admin");
    query.bindValue(4, "000");
    query.exec();

    query.bindValue(0, "111");
    query.bindValue(1, "MODERATOR");
    query.bindValue(2, 2);
    query.bindValue(3, "a_user");
    query.bindValue(4, "111");
    query.exec();

    query.bindValue(0, "222");
    query.bindValue(1, "COMMON_USER");
    query.bindValue(2, 1);
    query.bindValue(3, "b_user");
    query.bindValue(4, "222");
    query.exec();

    query.bindValue(0, "333");
    query.bindValue(1, "COMMON_USER");
    query.bindValue(2, 1);
    query.bindValue(3, "c_user");
    query.bindValue(4, "333");
    query.exec();

    query.bindValue(0, "444");
    query.bindValue(1, "COMMON_USER");
    query.bindValue(2, 1);
    query.bindValue(3, "d_user");
    query.bindValue(4, "444");
    query.exec();

    query.bindValue(0, "555");
    query.bindValue(1, "ADMINISTRATOR");
    query.bindValue(2, 0);
    query.bindValue(3, "b_admin");
    query.bindValue(4, "555");
    query.exec();
}

bool ForumStorage::InitiateBoards() {
    QSqlQuery query;

    //create boards table
    bool success = query.exec(
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
}

bool ForumStorage::InitiatePosts() {
    QSqlQuery query;

    //create posts table
    bool success = query.exec(
                "create table if not exists posts ("
                "id       varchar primary key,"
                "board    varchar,"
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
}

bool ForumStorage::InitiateComments() {
    QSqlQuery query;

    //create comments table
    bool success = query.exec(
                "create table if not exists comments ("
                "board    varchar,"
                "postid   varchar,"
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



/**************************NullStorage**********************/
ForumStorage& NullStorage::operator <<(QVector<QString>&) {
    throw QException();
}

ForumStorage& NullStorage::operator >>(QVector<QString>&) {
    throw QException();
}
