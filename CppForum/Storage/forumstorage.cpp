#include "forumstorage.h"
#include "Storage/userinfostorage.h"
#include "Storage/boardsstorage.h"
#include "Storage/postsstorage.h"
#include "Storage/commentsstorage.h"
#include <QSqlQuery>
#include <QDate>
#include <QException>
#include <QDebug>

ForumStorage::ForumStorage()
{

}

bool ForumStorage::InitiateStorage() {
    if(!InitiateUserInfo()||
       !InitiateBoards()  ||
       !InitiatePosts()   ||
       !InitiateComments()  ) {
        return false;
    }
    return true;
}

bool ForumStorage::InitiateUserInfo() {
    QSqlQuery query;
qDebug()<<"here";
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
    query.exec(); //if there is already a existing one, it returns false
//    if(!query.exec()){
//        qDebug()<<"insert failed";
//    }

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

    return true;
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

    query.prepare("insert into boards values (?,?)");

    query.bindValue(0, "C++11/14");
    query.bindValue(1, "111");
    query.exec();

    query.bindValue(0, "C++17");
    query.bindValue(1, "");
    query.exec();

    return true;
}

bool ForumStorage::InitiatePosts() {
    QSqlQuery query;

    //create posts table
    bool success = query.exec(
                "create table if not exists posts ("
                "id       varchar primary key,"
                "board    varchar,"
                "author   varchar,"
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

    query.prepare("insert into posts values (?,?,?,?,?,?,?)");

    query.bindValue(0, "0");
    query.bindValue(1, "C++11/14");
    query.bindValue(2, "c_user");
    query.bindValue(3, "333");
    query.bindValue(4, "lambda-calculus");
    query.bindValue(5, "Qt Creator's C++ compliler frontend doesn't support init capture");
    query.bindValue(6, QDate::currentDate().addDays(-5).toString());
    query.exec();

    query.bindValue(0, "1");
    query.bindValue(1, "C++11/14");
    query.bindValue(2, "b_user");
    query.bindValue(3, "222");
    query.bindValue(4, "memory model");
    query.bindValue(5, "What's new about C++'s memory model in the C++14 standard?");
    query.bindValue(6, QDate::currentDate().addDays(-4).toString());
    query.exec();

    query.bindValue(0, "2");
    query.bindValue(1, "C++11/14");
    query.bindValue(2, "a_user");
    query.bindValue(3, "111");
    query.bindValue(4, "std::packaged_task");
    query.bindValue(5, "The class template std::packaged_task wraps any Callable target (function, lambda expression, bind expression, or another function object) so that it can be invoked asynchronously. Its return value or exception thrown is stored in a shared state which can be accessed through std::future objects. ");
    query.bindValue(6, QDate::currentDate().addDays(-3).toString());
    query.exec();

    query.bindValue(0, "3");
    query.bindValue(1, "C++17");
    query.bindValue(2, "a_user");
    query.bindValue(3, "111");
    query.bindValue(4, "structured binding");
    query.bindValue(5, "I used structured binding in my C lexical analyzer!");
    query.bindValue(6, QDate::currentDate().addMonths(-1).toString());
    query.exec();

    query.bindValue(0, "4");
    query.bindValue(1, "C++17");
    query.bindValue(2, "d_user");
    query.bindValue(3, "444");
    query.bindValue(4, "file system");
    query.bindValue(5, "Can I use C++'s filesystem to implement a FUSE supported network storage?");
    query.bindValue(6, QDate::currentDate().addDays(-7).toString());
    query.exec();

    return true;
}

bool ForumStorage::InitiateComments() {
    QSqlQuery query;

    //create comments table
    bool success = query.exec(
                "create table if not exists comments ("
                "board    varchar,"
                "postid   varchar,"
                "author   varchar,"
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

    query.prepare("insert into comments values (?,?,?,?,?,?)");

    query.bindValue(0, "C++11/14");
    query.bindValue(1, "2");
    query.bindValue(2, "d_user");
    query.bindValue(3, "444");
    query.bindValue(4, "niubility!");
    query.bindValue(5, QDate::currentDate().addDays(-3).toString());
    query.exec();

    query.bindValue(0, "C++11/14");
    query.bindValue(1, "2");
    query.bindValue(2, "b_user");
    query.bindValue(3, "222");
    query.bindValue(4, "why do I need packaged_task when I have std::async?");
    query.bindValue(5, QDate::currentDate().addDays(-3).toString());
    query.exec();

    query.bindValue(0, "C++17");
    query.bindValue(1, "3");
    query.bindValue(2, "c_user");
    query.bindValue(3, "333");
    query.bindValue(4, "Awesome!");
    query.bindValue(5, QDate::currentDate().addDays(-15).toString());
    query.exec();

    query.bindValue(0, "C++17");
    query.bindValue(1, "3");
    query.bindValue(2, "b_user");
    query.bindValue(3, "222");
    query.bindValue(4, "Why my MSVC doesn's compile the C++17 code?");
    query.bindValue(5, QDate::currentDate().addDays(-10).toString());
    query.exec();

    query.bindValue(0, "C++17");
    query.bindValue(1, "4");
    query.bindValue(2, "a_user");
    query.bindValue(3, "111");
    query.bindValue(4, "I also want to know the answer...");
    query.bindValue(5, QDate::currentDate().addDays(-2).toString());
    query.exec();

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
