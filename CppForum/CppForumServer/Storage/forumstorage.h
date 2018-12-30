#ifndef FORUMSTORAGE_H
#define FORUMSTORAGE_H

#include <QSqlQuery>
#include <QString>
#include <QVector>
#include <cstdint>

class NullStorage;

class ForumStorage
{
public:
    enum Category: uint8_t
    {
        USERINFO,
        BOARDS,
        POSTS,
        COMMENTS
    };

public:
    ForumStorage();
    virtual ~ForumStorage() { }

    static bool InitiateStorage();
    static ForumStorage& GetStorage(const Category category);
    static ForumStorage& GetNullValue();

    //put data to database
    virtual ForumStorage& operator <<(QVector<QString>& record) =0;
    //retrieve data from database
    virtual ForumStorage& operator >>(QVector<QString>& record) =0;
    virtual operator bool() const;

    //update the #index field of record whose primary key is 'id'
    virtual bool UpdateRecord(const QString& id, int index, const QString& newVal) {
        return false;
    }
    virtual bool RemoveRecord(const QString& id) { return false; }
    virtual int NextId() { return 0; }

private:
    static bool InitiateUserInfo();
    static bool InitiateBoards();
    static bool InitiatePosts();
    static bool InitiateComments();

protected:
    QSqlQuery query;
    bool dataAvailable = false;
    enum { IN, OUT, NONE } lastOpration = NONE;
};


//represent null value for storage
class NullStorage: public ForumStorage
{
public:
    NullStorage() {}
    virtual ForumStorage& operator <<(QVector<QString>& record) override;
    virtual ForumStorage& operator >>(QVector<QString>& record) override;
    //operator bool() (inherited) returns false
};

#endif // FORUMSTORAGE_H
