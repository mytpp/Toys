#include "post.h"
#include "User/user.h"
#include "QDate"

Post::Post(const QString& id, const QString& poster,
           const QString& title, const QString& content,
           const QDate birthday, const std::list<Comment>& comments)
    :id(id)
    ,poster(poster)
    ,title(title)
    ,content(content)
    ,birthday(birthday)
    ,comments(comments)
{

}

bool Post::AddComment(const QString &content) const {
    comments.emplace_back(User::Get()->Id(),
                          content,
                          QDate::currentDate());
    return true;
}
