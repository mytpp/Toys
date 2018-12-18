#include "board.h"
#include "User/user.h"
#include <QDate>
#include <iterator>
#include <algorithm>
#include "Storage/postsstorage.h"


Board::Board(const QString &name, const QString &moderatorId)
    :name(name)
    ,moderatorId(moderatorId)
{ }

void Board::AddInitialComment(const Comment &comment, const QString& postId) {
    auto it = posts.find(postId);
    if(it == posts.end())
        return;
    it->second.AddInitialComment(comment);
}

Post& Board::AddPost(const QString &title, const QString &content){
    //add post to db
    auto& storage = ForumStorage::GetStorage(ForumStorage::POSTS);
    QVector<QString> record;
    auto id        = QString().setNum(storage.NextId());
    auto& userId   = User::Get()->Id();
    auto& userName = User::Get()->Name();
    auto  now      = QDate::currentDate();
    record<<id
          <<this->Name()
          <<userName
          <<userId
          <<title
          <<content
          <<now.toString();
    storage<<record;

    User::Get()->AddPost();
    auto retVal = posts.emplace(id, Post{
                    id,
                    userName,
                    userId,
                    title,
                    content,
                    now
                }
            );
    return retVal.first->second;
}

bool Board::DeletePost(const QString id) {
    auto it = posts.find(id);
    if(it == posts.end())
        return false;
    auto& storage = ForumStorage::GetStorage(ForumStorage::POSTS);
    if(!storage.RemoveRecord(it->second.Id()))
        return false;
    posts.erase(it);
    User::Get()->DeletePost();
    return true;
}
