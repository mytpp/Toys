#include "comment.h"

Comment::Comment(const QString &author,
                 const QString &authorId,
                 const QString &content,
                 const QDate birthday)
    :author(author)
    ,authorId(authorId)
    ,content(content)
    ,birthday(birthday)
{ }

