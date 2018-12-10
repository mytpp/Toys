#include "comment.h"

Comment::Comment(const QString& author,
                 const QString& content,
                 const QDate birthday)
    :authorId(author)
    ,content(content)
    ,birthday(birthday)
{

}

Comment::Comment(const QString &author,
                 const QString &authorId,
                 const QString &content,
                 const QDate birthday)
    :author(author)
    ,authorId(authorId)
    ,content(content)
    ,birthday(birthday)
{ }

int Comment::CreateCommentId() {
    static int value = 10;
    return value++;
}
