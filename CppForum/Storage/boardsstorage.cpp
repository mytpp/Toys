#include "boardsstorage.h"


BoardsStorage::BoardsStorage()
{

}

ForumStorage& BoardsStorage::operator <<(QVector<QString>& record) {
    //QSqlQuery
    return ForumStorage::GetNullValue();
}

ForumStorage& BoardsStorage::operator >>(QVector<QString>& record) {
    return ForumStorage::GetNullValue();
}
