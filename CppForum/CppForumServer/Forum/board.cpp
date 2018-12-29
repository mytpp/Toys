#include "board.h"
#include <QDate>
#include <iterator>
#include <algorithm>
#include "Storage/postsstorage.h"
#include "Forum/forum.h"


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

std::map<QString, Post> Board::GetPosts() const {
    std::scoped_lock lock(postsMutex);
    return posts; // copy it, or posts may be changed in other thread
}

std::pair<bool, QString> Board::AddPost(const QString &title, const QString &content,
                     const QString& author, const QString& authorId){
    std::scoped_lock lock(postsMutex);
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
        Forum::Get().IncPostCountOf(authorId);
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

bool Board::DeletePost(const QString& id) {
    std::scoped_lock lock(postsMutex);
    auto it = posts.find(id);
    if(it == posts.end())
        return false;
    auto& storage = ForumStorage::GetStorage(ForumStorage::POSTS);
    if(!storage.RemoveRecord(it->second.Id()))
        return false;
    Forum::Get().DecPostCountOf(it->second.AuthorId());
    posts.erase(it);
    return true;
}

bool Board::AddComment(const QString &postId, const QString &content,
                       const QString &author, const QString &authorId) {
    std::scoped_lock lock(postsMutex);
    auto it = posts.find(postId);
    if(it == posts.end())
        return false;
    return it->second.AddComment(content, author, authorId);
}
