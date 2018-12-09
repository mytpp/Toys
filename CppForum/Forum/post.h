#ifndef POST_H
#define POST_H

#include "Forum/comment.h"
#include "infrastructure.h"

class Post final
{
public:
    Post(const QString& id, const QString& author, const QString& authorId,
         const QString& title, const QString& content, const QDate birthday,
         const std::list<Comment>& comments = {});
    void AddInitialComment(const Comment& comment) {comments.push_back(comment);}
    //add itself to VBoxlayout
    void Show(infrastructure::Status status = infrastructure::Status::ANONYMOUS);

    void ShowComments();

    const QString& Id()       const { return id; }
    const QString& Poster()   const { return author; }
    const QString& AuthorId() const { return authorId; }
    const QString& Title()    const { return title; }
    const QString& Content()  const { return content; }
    const QDate&   Date()     const { return birthday; }

    const std::list<Comment>& Comments() const { return comments; }
    bool AddComment(const QString& content);

    static int CreatePostId();

private:
    QString id;
    QString author;
    QString authorId;
    QString title;
    QString content;
    QDate birthday;

    std::list<Comment> comments;
};

#endif // POST_H
