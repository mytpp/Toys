#include "commentsstorage.h"
#include <QSqlQuery>

CommentsStorage::CommentsStorage()
{

}

ForumStorage& CommentsStorage::operator <<(QVector<QString>& record) {
    //QSqlQuery
    return ForumStorage::GetNullValue();
}

ForumStorage& CommentsStorage::operator >>(QVector<QString>& record) {
    return ForumStorage::GetNullValue();
}
