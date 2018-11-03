#ifndef POSTSAREA_H
#define POSTSAREA_H

#include <QVBoxLayout>
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

    QFrame *newPostArea;
    QTextEdit *contentEdit;
    QLineEdit *titleEdit;
    QPushButton *postButton;
};

#endif // POSTSAREA_H
