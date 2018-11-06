#ifndef POSTSAREA_H
#define POSTSAREA_H

#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QScrollArea>
#include <QPushButton>
#include <QList>

class PostComponent;

class PostsArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit PostsArea(QWidget *parent = nullptr);

public slots:
    void OnDeletePost(int index);

private:
    QVBoxLayout *postsLayout;

    QPushButton *assignModeratorButton;

    QList<PostComponent*> postComponents;

    //An area to add new post to the forum is needed.
    //That's what the following variables for
    QFrame *newPostArea;
    QLabel *titleLabel;
    QLineEdit *titleEdit;
    QLabel *contentLabel;
    QTextEdit *contentEdit;
    QPushButton *postButton;
};

#endif // POSTSAREA_H
