#include "post.h"

Post::Post(const QString& id, const QString& poster, const QString& title,
           const QString& content, QDate birthday)
    :id(id)
    ,poster(poster)
    ,title(title)
    ,content(content)
    ,birthday(birthday)
{

}
