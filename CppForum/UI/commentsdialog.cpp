#include <algorithm>
#include <QTextBrowser>
#include <QHBoxLayout>
#include <QLabel>
#include "commentsdialog.h"
#include "Forum/post.h"
#include <QDebug>

CommentsDialog::CommentsDialog(const Post& post, QWidget *parent)
    :QDialog(parent)
{
qDebug()<<"in com dlg";
    commentLayout = new QVBoxLayout(this);
    commentLayout->setSpacing(20);

    auto& comments = post.Comments();
    std::for_each(comments.begin(), comments.end(),
                  [=, &comments](const Comment& comment){
        auto commentBlock = new QTextBrowser(this);
        commentBlock->setText(comment.Content());
        commentLayout->addWidget(commentBlock);

        auto innerLayout = new QHBoxLayout();
        innerLayout->addStretch();
        auto author = new QLabel(comment.AuthorId());
        innerLayout->addWidget(author);
        auto date = new QLabel(comment.PublishDate().toString());
        innerLayout->addWidget(date);

        commentLayout->addLayout(innerLayout);
    });


}
