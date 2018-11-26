#include "board.h"
#include "User/user.h"
#include <QDate>
#include <iterator>

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

bool Board::AddPost(const QString& guid, const QString &title, const QString &content){
    posts.emplace_back(
        guid,
        User::Get()->Id(),
        title,
        content,
        QDate::currentDate()
    );
    User::Get()->AddPost();
    return true;
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
