#ifndef BOARDSSTORAGE_H
#define BOARDSSTORAGE_H

#include "forumstorage.h"

class BoardsStorage : public ForumStorage
{
public:
    BoardsStorage();

    virtual ForumStorage& operator <<(QVector<QString>& record) override;
    virtual ForumStorage& operator >>(QVector<QString>& record) override;

    virtual bool UpdateRecord(const QString& id,
                              int index, const QString &newVal) override;

};

#endif // BOARDSSTORAGE_H
