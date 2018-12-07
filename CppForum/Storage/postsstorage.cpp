#include "postsstorage.h"
#include <QSqlQuery>

PostsStorage::PostsStorage()
{

}

ForumStorage& PostsStorage::operator <<(QVector<QString>& record) {
    //QSqlQuery
    return ForumStorage::GetNullValue();
}

ForumStorage& PostsStorage::operator >>(QVector<QString>& record) {
    return ForumStorage::GetNullValue();
}
