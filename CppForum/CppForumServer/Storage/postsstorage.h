#ifndef POSTSSTORAGE_H
#define POSTSSTORAGE_H

#include "forumstorage.h"
#include <atomic>

class PostsStorage : public ForumStorage
{
public:
    PostsStorage();

    virtual ForumStorage& operator <<(QVector<QString>& record) override;
    virtual ForumStorage& operator >>(QVector<QString>& record) override;

    virtual bool RemoveRecord(const QString &id) override;
    virtual int NextId() override { return nextId++; }

private:
    //operation to abtain next post id must be atomic
    std::atomic<int> nextId = 0;
};

#endif // POSTSSTORAGE_H
