#ifndef POSTSAREA_H
#define POSTSAREA_H

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QList>

class PostComponent;
class PostEdit;

class PostsArea: public QWidget
{
    Q_OBJECT
public:
    explicit PostsArea(QWidget *parent = nullptr);

public slots:
    void OnDeletePost(int index);
    void OnAddPost(const QString& title, const QString& content);

private:
    QVBoxLayout *postsLayout;
    QList<PostComponent*> postComponents;
    PostEdit *postEdit;
};

#endif // POSTSAREA_H
