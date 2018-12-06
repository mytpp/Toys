#ifndef POSTSSTORAGE_H
#define POSTSSTORAGE_H

#include "forumstorage.h"

class PostsStorage : public ForumStorage
{
public:
    PostsStorage();

    //put data to database
    virtual ForumStorage& operator <<(QVector<QString>& record) override;
    //retrieve data from database
    virtual ForumStorage& operator >>(QVector<QString>& record) override;
};

#endif // POSTSSTORAGE_H
