#include "board.h"
#include "User/user.h"
#include <QDate>
#include <iterator>

Board::Board(const QString& name, const QString moderatorId)
    :name(name)
    ,moderatorId(moderatorId)
{

}

bool Board::AddPost(const QString& guid, const QString &title, const QString &content){
    posts.emplace_back(
        guid,
        User::Get()->Id(),
        title,
        content,
        QDate::currentDate()
    );
    return true;
}

bool Board::DeletePost(size_t index) {
    if(index >= posts.size())
        return false;
    auto it = posts.begin();
    std::advance(it, index);
    posts.erase(it);
    return true;
}
