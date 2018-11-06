#ifndef COMMENT_H
#define COMMENT_H

#include <QString>
#include <QDate>

class Comment
{
public:
    Comment(const QString& author,
            const QString& content,
            const QDate birthday);

private:
    QString author;
    QString content;
    QDate birthday;
};

#endif // COMMENT_H
