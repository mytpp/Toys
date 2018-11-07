#ifndef POSTCOMPONENT_H
#define POSTCOMPONENT_H

#include <QWidget>
#include <QTextBrowser>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "infrastructure.h"

class Post;
class CommentsDialog;

class PostComponent : public QWidget
{
    Q_OBJECT
public:
    explicit PostComponent(QWidget *parent = nullptr);
    explicit PostComponent(const Post& post, const int index, QWidget *parent = nullptr);

signals:
    void DeletePostAtIndex(int index);

private:
    int index;  //the offset of this component at PostArea
    QLabel *title;
    QTextBrowser *content;
    QPushButton *comments;
    QPushButton *deletePost;

    QVBoxLayout *vLayout;
    QHBoxLayout *hLayout;

    CommentsDialog *commentsDialog;
};

#endif // POSTCOMPONENT_H
