#ifndef POST_H
#define POST_H

#include "Forum/comment.h"
#include "infrastructure.h"

class Post final
{
public:
    Post(const QString& id, const QString& author,
         const QString& authorId, const QString& title,
         const QString& content, const QDate birthday,
         const int commentsNum = 0);

    const QString& Id()          const { return id; }
    const QString& Poster()      const { return author; }
    const QString& AuthorId()    const { return authorId; }
    const QString& Title()       const { return title; }
    const QString& Content()     const { return content; }
    const QDate&   Date()        const { return birthday; }
    const int      CommentsNum() const { return commentsNum; }

    const std::list<Comment> Comments() const;
    bool AddComment(const QString& content);

private:
    QString id;
    QString author;
    QString authorId;
    QString title;
    QString content;
    QDate birthday;
    int commentsNum;
};

#endif // POST_H
