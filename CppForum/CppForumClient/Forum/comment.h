#ifndef COMMENT_H
#define COMMENT_H

#include <QString>
#include <QDate>

class Comment
{
public:
    Comment(const QString &author,
            const QString &authorId,
            const QString &content,
            const QDate birthday);

    const QString& Author()      const { return author;   }
    const QString& AuthorId()    const { return authorId; }
    const QString& Content()     const { return content;  }
    const QDate&   PublishDate() const { return birthday; }

private:
    QString author;
    QString authorId;
    QString content;
    QDate birthday;
};

#endif // COMMENT_H
