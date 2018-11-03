#ifndef COMMENT_H
#define COMMENT_H

#include <QObject>
#include <QString>
#include <QDate>

class Comment
{
public:
    Comment();

private:
    QString id;
    QString content;
    QDate birthday;
};

#endif // COMMENT_H
