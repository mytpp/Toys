#ifndef COMMENTSSTORAGE_H
#define COMMENTSSTORAGE_H

#include "forumstorage.h"

class CommentsStorage : public ForumStorage
{
public:
    CommentsStorage();

    virtual ForumStorage& operator <<(QVector<QString>& record) override;
    virtual ForumStorage& operator >>(QVector<QString>& record) override;
};

#endif // COMMENTSSTORAGE_H
