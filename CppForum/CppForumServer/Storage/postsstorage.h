#ifndef POSTSSTORAGE_H
#define POSTSSTORAGE_H

#include "forumstorage.h"

class PostsStorage : public ForumStorage
{
public:
    PostsStorage();

    virtual ForumStorage& operator <<(QVector<QString>& record) override;
    virtual ForumStorage& operator >>(QVector<QString>& record) override;

    virtual bool RemoveRecord(const QString &id) override;
    virtual int NextId() override { return nextId++; }

private:
    int nextId = 0;
};

#endif // POSTSSTORAGE_H
