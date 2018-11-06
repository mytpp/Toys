#ifndef POST_H
#define POST_H

#include "Forum/comment.h"
#include "infrastructure.h"

class Post final
{
public:
    Post(const QString& id, const QString& poster,
         const QString& title, const QString& content,
         const QDate birthday, const std::list<Comment>& comments = {});

    //add itself to VBoxlayout
    void Show(infrastructure::Status status = infrastructure::Status::ANONYMOUS);

    void ShowComments();

    const QString& Id()     const { return id; }
    const QString& Poster() const { return poster; }
    const QString& Title()  const { return title; }
    const QString& Content()const { return content; }
    const QDate&   Date()   const { return birthday; }

    const std::list<Comment>& Comments() const { return comments; }

private:
    QString id;
    QString poster;
    QString title;
    QString content;
    QDate birthday;

    std::list<Comment> comments;
};

#endif // POST_H
