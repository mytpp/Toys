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
    explicit PostComponent(Post& post, const int index, QWidget *parent = nullptr);
    void DecIndex() { --index; }
    //void IncIndex() { ++index; }

signals:
    void DeletePostAtIndex(int index);

private:
    int index;  //the offset of this component at PostArea
    QLabel *title = nullptr;
    QTextBrowser *content = nullptr;
    int commentsCount;
    QPushButton *comments = nullptr;
    QPushButton *deletePost = nullptr;

    QVBoxLayout *vLayout = nullptr;
    QHBoxLayout *hLayout = nullptr;

    CommentsDialog *commentsDialog = nullptr;
};

#endif // POSTCOMPONENT_H
