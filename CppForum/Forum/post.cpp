#include "post.h"
#include "User/user.h"
#include "QDate"

Post::Post(const QString& id, const QString& author, const QString& authorId,
           const QString& title, const QString& content, const QDate birthday,
           const std::list<Comment>& comments)
    :id(id)
    ,author(author)
    ,authorId(authorId)
    ,title(title)
    ,content(content)
    ,birthday(birthday)
    ,comments(comments)
{

}

bool Post::AddComment(const QString &content) {
    comments.emplace_back(User::Get()->Id(),
                          content,
                          QDate::currentDate());
    return true;
}

int Post::CreatePostId() {
    static int value = 10;
    return value++;
}
