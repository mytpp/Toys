#ifndef USERINFOSTORAGE_H
#define USERINFOSTORAGE_H

#include "forumstorage.h"


class UserInfoStorage : public ForumStorage
{
public:
    UserInfoStorage();

    virtual ForumStorage& operator <<(QVector<QString>& record) override;
    virtual ForumStorage& operator >>(QVector<QString>& record) override;
};

#endif // USERINFOSTORAGE_H
