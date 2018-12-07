#ifndef POSTSSTORAGE_H
#define POSTSSTORAGE_H

#include "forumstorage.h"

class PostsStorage : public ForumStorage
{
public:
    PostsStorage();

    virtual ForumStorage& operator <<(QVector<QString>& record) override;
    virtual ForumStorage& operator >>(QVector<QString>& record) override;
};

#endif // POSTSSTORAGE_H
