#ifndef USERINFOSTORAGE_H
#define USERINFOSTORAGE_H

#include "forumstorage.h"


class UserInfoStorage : public ForumStorage
{
public:
    UserInfoStorage();

    virtual ForumStorage& operator <<(QVector<QString>& record) override;
    virtual ForumStorage& operator >>(QVector<QString>& record) override;

    virtual bool UpdateRecord(const QString& id,
                              int index, const QString &newVal) override;
    virtual bool RemoveRecord(const QString &id) override;
};

#endif // USERINFOSTORAGE_H
