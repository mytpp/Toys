#ifndef COMMENTSSTORAGE_H
#define COMMENTSSTORAGE_H

#include "forumstorage.h"

class CommentsStorage : public ForumStorage
{
public:
    CommentsStorage();

    virtual ForumStorage& operator <<(QVector<QString>& record) override;
    virtual ForumStorage& operator >>(QVector<QString>& record) override;
    virtual int NextId() override { return nextId++; }

private:
    int nextId = 0;
};

#endif // COMMENTSSTORAGE_H
