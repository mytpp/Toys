#include "postcomponent.h"
#include "User/user.h"
#include "Forum/post.h"
#include "Forum/forum.h"
#include "UI/postsarea.h"
#include "UI/commentsdialog.h"
#include <QDebug>


PostComponent::PostComponent(Post &post, const int index, QWidget *parent)
    :QWidget(parent)
    ,index(index)
    ,id(post.Id())
{
    vLayout = new QVBoxLayout(this);
    hLayout = new QHBoxLayout();

    //title
    title = new QLabel(this);
    title->setText(post.Title() + tr("(Posted by ") +
                   post.Poster() + tr(")") );
    vLayout->addWidget(title);

    //content
    content = new QTextBrowser(this);
    content->setText(post.Content());
    content->setMaximumHeight(200);
    vLayout->addWidget(content);

    //comment button
    comments = new QPushButton(this);
    commentsCount = post.CommentsNum();
    comments->setText(QString::number(commentsCount) + tr(" Comments"));
    hLayout->addStretch();   //placeholder to make button align right
    hLayout->addWidget(comments);
    vLayout->addLayout(hLayout);

    //connect 'comment' button to commentsdialog
    connect(comments, &QPushButton::clicked,
            [=, &post]{
        commentsDialog = new CommentsDialog(post, this);
        commentsDialog->show();
        connect(commentsDialog, &CommentsDialog::AddComment, [=]{
            comments->setText(QString::number(++commentsCount) + tr(" Comments"));
            auto status = User::Get()->GetProfile().status;
            if(deletePost && status != infrastructure::MODERATOR) {
                delete deletePost;
                deletePost = nullptr;
            }
        });
    });

    //"Delete" button conditional appears
    //check if the requirement is met
    auto&& status = User::Get()->GetProfile().status;
    auto& curModerator = Forum::Get().GetCurBoard().ModeratorId();

    if(status == infrastructure::MODERATOR
          && User::Get()->Id() == curModerator
       || post.CommentsNum() == 0
          && (status == infrastructure::COMMON_USER
              || status == infrastructure::MODERATOR)
          && User::Get()->Id() == post.AuthorId()
    ) {
        deletePost = new QPushButton(this);
        deletePost->setText(tr("Delete"));
        hLayout->addWidget(deletePost);

        //link delete button to another signal to pass parameter
        connect(deletePost, &QPushButton::clicked, [this]{
            if(Forum::Get().GetCurBoard().DeletePost(this->Id())) {
                //send this signal need 'this' pointer
                emit DeletePostAtIndex(this->index);
            }
        });

        //reemit the signal to pass a paramater
        connect(this,
                &PostComponent::DeletePostAtIndex,
                qobject_cast<PostsArea*>(this->parent()),
                &PostsArea::OnDeletePost);
    }//end if
}


