#include "post.h"
#include "User/user.h"
#include <QDate>
#include "Storage/commentsstorage.h"
#include "Forum/forum.h"

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
    auto& storage = ForumStorage::GetStorage("comments");
    QVector<QString> record;
    record<<QString().setNum(storage.NextId())
          <<Forum::Get().GetCurBoard().Name()
          <<this->Id()
          <<User::Get()->Name()
          <<User::Get()->Id()
          <<content
          <<QDate::currentDate().toString();
    if(!(storage<<record)) {
        qDebug()<<"Add comment to db failed";
        return false;
    }

    comments.emplace_back(User::Get()->Id(),
                          content,
                          QDate::currentDate());
    return true;
}
