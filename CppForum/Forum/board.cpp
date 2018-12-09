#include "board.h"
#include "User/user.h"
#include <QDate>
#include <iterator>
#include <algorithm>

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

Post& Board::AddPost(const QString& guid, const QString &title, const QString &content){
    User::Get()->AddPost();
    return posts.emplace_back( //need c++17 supported
        guid,
        User::Get()->Name(),
        User::Get()->Id(),
        title,
        content,
        QDate::currentDate()
    );
}

bool Board::DeletePost(size_t index) {
    if(index >= posts.size())
        return false;
    auto it = posts.begin();
    std::advance(it, index);
    posts.erase(it);
    User::Get()->DeletePost();
    return true;
}
