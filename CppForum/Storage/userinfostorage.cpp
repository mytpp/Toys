#include "userinfostorage.h"
#include <QSqlQuery>

UserInfoStorage::UserInfoStorage()
{

}

ForumStorage& UserInfoStorage::operator <<(QVector<QString>& record) {
    QSqlQuery query;

    return ForumStorage::GetNullValue();
}

ForumStorage& UserInfoStorage::operator >>(QVector<QString>& record) {
    return ForumStorage::GetNullValue();
}
