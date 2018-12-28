#include "post.h"
#include <QDate>
#include "Storage/commentsstorage.h"
#include "Forum/forum.h"

Post::Post(const QString& id, const QString& author, const QString& authorId,
           const QString& title, const QString& content, const QDate birthday,
           const QString& board)
    :id(id)
    ,author(author)
    ,authorId(authorId)
    ,title(title)
    ,content(content)
    ,birthday(birthday)
    ,board(board)
{

}

bool Post::AddComment(const QString &content,
                      const QString& author,
                      const QString& authorId) {
    auto& storage = ForumStorage::GetStorage(ForumStorage::COMMENTS);
    QVector<QString> record;
    record<<QString().setNum(storage.NextId())
          <<board
          <<this->Id()
          <<author
          <<authorId
          <<content
          <<QDate::currentDate().toString();
    if(!(storage<<record)) {
        qInfo()<<"Add comment to db failed";
        return false;
    }

    comments.emplace_back(author,
                          authorId,
                          content,
                          QDate::currentDate());
    return true;
}
