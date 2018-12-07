#ifndef FORUMSTORAGE_H
#define FORUMSTORAGE_H


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

    virtual ForumStorage& operator <<(QVector<QString>& record) =0;
    virtual ForumStorage& operator >>(QVector<QString>& record) =0;
};

class NullStorage: public ForumStorage
{
public:
    NullStorage() {}
    virtual ForumStorage& operator <<(QVector<QString>& record) override;
    virtual ForumStorage& operator >>(QVector<QString>& record) override;
};

#endif // FORUMSTORAGE_H
