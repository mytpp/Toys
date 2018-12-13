#include "board.h"
#include "User/user.h"
#include <QDate>
#include <iterator>
#include <algorithm>
#include "Storage/postsstorage.h"

Board::Board(const QString &name, const std::list<Post> &posts)
    :name(name)
    ,posts(posts)
{ }

Board::Board(const QString &name, const QString &moderatorId,
             const std::list<Post>& posts)
    :name(name)
    ,moderatorId(moderatorId)
    ,posts(posts)
{ }

void Board::AddInitialComment(const Comment &comment, const QString& postId) {
    auto p = std::find_if(posts.begin(), posts.end(),
                          [&postId](auto& post) {return postId == post.Id();});
    if(p != posts.end())
        p->AddInitialComment(comment);
}

Post& Board::AddPost(const QString &title, const QString &content){
    //add post to db
    auto& storage = ForumStorage::GetStorage("posts");
    QVector<QString> record;
    auto&& id      = QString().setNum(storage.NextId());
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
    return posts.emplace_back( //need c++17 supported
        id,
        userName,
        userId,
        title,
        content,
        now
    );
}

bool Board::DeletePost(size_t index) {
    if(index >= posts.size())
        return false;
    auto& storage = ForumStorage::GetStorage("posts");
    auto it = posts.begin();
    std::advance(it, index);
    if(!storage.RemoveRecord(it->Id()))
        return false;
    posts.erase(it);
    User::Get()->DeletePost();
    return true;
}
