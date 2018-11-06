#include "post.h"

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
