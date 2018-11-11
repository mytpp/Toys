#ifndef COMMENTSDIALOG_H
#define COMMENTSDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QLineEdit>
#include <QList>

class Post;

class CommentsDialog : public QDialog
{
    Q_OBJECT
public:
    CommentsDialog(const Post& post, QWidget *parent = 0);

signals:
    void AddComment();

private:
    void AddGraphicalComment(const QString& author,
                             const QString& date,
                             const QString& content);
    void OnAddComment(const QString& content);
    void CreateCommentEdit();

private:
    const Post& post;

    QVBoxLayout *dialogLayout;
    QVBoxLayout *commentLayout;
    QScrollArea *scrollArea;
    QFrame *commentsArea;

    //Area to add new comment
    QLineEdit *commentEdit;
    QPushButton *publishButton;
};

#endif // COMMENTSDIALOG_H
