#ifndef COMMENTSDIALOG_H
#define COMMENTSDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QList>

class Post;

class CommentsDialog : public QDialog
{
public:
    CommentsDialog(const Post& post, QWidget *parent = 0);

private:
    QVBoxLayout *commentLayout;

};

#endif // COMMENTSDIALOG_H
