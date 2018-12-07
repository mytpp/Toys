#ifndef FORUMSTORAGE_H
#define FORUMSTORAGE_H

#include <QSqlQuery>
#include <QString>
#include <QVector>

class NullStorage;

class ForumStorage
{
public:
    ForumStorage();
    virtual ~ForumStorage() { }

    static bool InitiateStorage();
    static ForumStorage& GetStorage(const QString& category);
    static ForumStorage& GetNullValue();

    //put data to database
    virtual ForumStorage& operator <<(QVector<QString>& record) =0;
    //retrieve data from database
    virtual ForumStorage& operator >>(QVector<QString>& record) =0;
    virtual operator bool() const { return false; }

private:
    static bool InitiateUserInfo();
    static bool InitiateBoards();
    static bool InitiatePosts();
    static bool InitiateComments();

protected:
    QSqlQuery query;
};


//represent null value for storage
class NullStorage: public ForumStorage
{
public:
    NullStorage() {}
    virtual ForumStorage& operator <<(QVector<QString>& record) override;
    virtual ForumStorage& operator >>(QVector<QString>& record) override;
};

#endif // FORUMSTORAGE_H
