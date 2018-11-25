#include "postedit.h"
#include "UI/postsarea.h"
#include <QGridLayout>
#include <QDebug>

PostEdit::PostEdit(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(500, 200);

    //draw the area to add new post
    titleLabel = new QLabel(tr("Title:"));
    contentEdit = new QTextEdit(this);
    contentEdit->setMaximumHeight(120);
    contentLabel = new QLabel(tr("Content:"));
    titleEdit = new QLineEdit(this);
    postButton = new QPushButton(tr("Post"), this);

    auto newPostLayout = new QGridLayout(this);
    newPostLayout->addWidget(titleLabel, 0, 0);
    newPostLayout->addWidget(titleEdit, 0, 1);
    newPostLayout->addWidget(contentLabel, 1, 0);
    newPostLayout->addWidget(contentEdit, 1, 1);
    newPostLayout->addWidget(postButton, 2, 1);


    connect(postButton, &QPushButton::clicked, [=]{
        auto content = contentEdit->toPlainText();
        auto title = titleEdit->text();
        emit AddPost(title, content);
    });

    connect(this,
            &PostEdit::AddPost,
            qobject_cast<PostsArea*>(this->parent()),
            &PostsArea::OnAddPost);
}
