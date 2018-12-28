#include "board.h"
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

std::pair<bool, QString> Board::AddPost(const QString &title, const QString &content,
                     const QString& author, const QString& authorId){
    //add post to db
    auto& storage = ForumStorage::GetStorage(ForumStorage::POSTS);
    auto  id       = QString().setNum(storage.NextId());
    auto  now      = QDate::currentDate();
    QVector<QString> record;
    record<<id
          <<this->Name()
          <<author
          <<authorId
          <<title
          <<content
          <<now.toString();

    if(storage<<record){
        posts.emplace(id, Post {
                            id,
                            author,
                            authorId,
                            title,
                            content,
                            now,
                            this->Name()
                        }
        );
        return {true, id};
    }
    qInfo()<<"file:"<<__FILE__<<" line:"<<__LINE__<<" Add post failed";
    return {false, ""};
}

bool Board::DeletePost(const QString id) {
    auto it = posts.find(id);
    if(it == posts.end())
        return false;
    auto& storage = ForumStorage::GetStorage(ForumStorage::POSTS);
    if(!storage.RemoveRecord(it->second.Id()))
        return false;
    posts.erase(it);
    return true;
}
