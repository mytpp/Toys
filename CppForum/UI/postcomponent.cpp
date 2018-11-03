#include "postcomponent.h"
#include "User/user.h"
#include "Forum/post.h"
#include "Forum/forum.h"
#include "UI/postsarea.h"


PostComponent::PostComponent(QWidget *parent) : QWidget(parent)
{

}

PostComponent::PostComponent(const Post &post, const int index, QWidget *parent)
    :QWidget(parent)
    ,index(index)
{
    vLayout = new QVBoxLayout(this);
    hLayout = new QHBoxLayout(this);

    //title
    title = new QLabel(this);
    title->setText(post.Title());
    vLayout->addWidget(title);

    //content
    content = new QTextBrowser(this);
    content->setText(post.Content());
    content->setMinimumWidth(400);
    content->setMinimumHeight(150);
    vLayout->addWidget(content);

    //comment
    comments = new QPushButton(this);
    auto commentsCount = post.Comments().size();
    comments->setText(QString::number(commentsCount) + tr(" Comments"));

    //"Delete" button conditional appears
    auto&& status = User::Get()->GetProfile().status;
    auto& curModerator = Forum::Get().GetCurBoard().ModeratorId();
    if( status == infrastructure::MODERATOR
            && User::Get()->Id() == curModerator
        || status == infrastructure::COMMON_USER
            && User::Get()->Id() == post.Poster()) {
        deletePost = new QPushButton(this);
        deletePost->setText(tr("Delete"));

        //link delete button to another signal to pass parameter
        connect(deletePost, &QPushButton::clicked,
                [index = index, this]{
            if(Forum::Get().GetCurBoard().DeletePost(index)) {
                //send this signal need 'this' pointer
                emit DeletePostAtIndex(index);
            }
        });

        connect(this, &PostComponent::DeletePostAtIndex,
                qobject_cast<PostsArea*>(this->parent()),
                &PostsArea::OnDeletePost);
    }
}


