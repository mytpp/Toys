#include "comment.h"

Comment::Comment(const QString& author,
                 const QString& content,
                 const QDate birthday)
    :author(author)
    ,content(content)
    ,birthday(birthday)
{

}
