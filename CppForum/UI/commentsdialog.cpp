#include <algorithm>
#include <QTextBrowser>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include "commentsdialog.h"
#include "Forum/post.h"
#include "User/user.h"
#include <QDebug>

CommentsDialog::CommentsDialog(const Post& post, QWidget *parent)
    :QDialog(parent),
      post(post)
{
qDebug()<<"in com dlg";
    commentsArea = new QFrame();
    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(commentsArea);
    //Enable 'Resizable' so that we can add widgets afterwards,
    //and the scrollArea grows at the same time.
    //(Given that the initial size of scrollArea is 0)
    scrollArea->setWidgetResizable(true);

    //make scrollArea fit the size of the dialog
    dialogLayout = new QVBoxLayout(this);
    dialogLayout->addWidget(scrollArea);

    commentLayout = new QVBoxLayout(commentsArea);
    commentLayout->setSpacing(20);

    auto& comments = post.Comments();
    std::for_each(comments.begin(), comments.end(),
                  [=, &comments](const Comment& comment){
        //visualize each comment
        AddGraphicalComment(
                    comment.AuthorId(),
                    comment.PublishDate().toString(),
                    comment.Content()
        );
    });

    commentLayout->addStretch();

    auto status = User::Get()->GetProfile().status;
    if(status == infrastructure::COMMON_USER ||
        status == infrastructure::MODERATOR) {
        CreateCommentEdit();
    }

}

void CommentsDialog::AddGraphicalComment(const QString &author,
                                         const QString &date,
                                         const QString &content) {
    //inner layout contain the author and publish date of a comment
    auto innerLayout = new QHBoxLayout();

    auto authorLabel = new QLabel(author); //author label
    innerLayout->addWidget(authorLabel);
    innerLayout->addStretch();             //for spacing
    auto dateLabel = new QLabel(date);     //publish date label
    innerLayout->addWidget(dateLabel);

    commentLayout->addLayout(innerLayout);

    auto commentBlock = new QTextBrowser();//comment content
    commentBlock->setText(content);
    commentLayout->addWidget(commentBlock);
}


void CommentsDialog::OnAddComment(const QString &content) {
    //check content
    if(content.isEmpty()) {
        QMessageBox::warning(nullptr,
                             tr("Warning"),
                             tr("Type Some Text to Comment!"),
                             QMessageBox::Ok);
        return;
    }

    if(post.AddComment(content)) {
        //refresh the botton area
        delete publishButton;
        delete commentEdit;
        AddGraphicalComment(User::Get()->Id(),
                            QDate::currentDate().toString(),
                            content);
        CreateCommentEdit();
    }
}

void CommentsDialog::CreateCommentEdit() {
    commentEdit = new QLineEdit();
    publishButton = new QPushButton(tr("Add new comment"));
    commentLayout->addWidget(commentEdit);
    commentLayout->addWidget(publishButton);

    connect(publishButton, &QPushButton::clicked, [=]{
        OnAddComment(commentEdit->text());
    });
}
