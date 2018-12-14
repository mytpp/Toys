#include "postsarea.h"
#include "UI/postcomponent.h"
#include "UI/postedit.h"
#include "Forum/forum.h"
#include <QMessageBox>
#include "Forum/post.h"
#include "User/user.h"
#include "forumui.h"


PostsArea::PostsArea(QWidget *parent) : QWidget(parent)
{
    postsLayout = new QVBoxLayout(this);
    postsLayout->setMargin(20);


    //retrive posts from the Forum
    auto& posts = Forum::Get().GetCurBoard().GetPosts();

    int index = 0;
    //add Posts to this area
    for(auto& postIt: posts) {
        auto* postComponent = new PostComponent(postIt.second, index++, this);
        postComponents.append(postComponent);
        postsLayout->addWidget(postComponent);
    }

    //check whether the new post area can be shown
    auto status = User::Get()->GetProfile().status;
    if(status == infrastructure::COMMON_USER ||
            status == infrastructure::MODERATOR) {
        postEdit = new PostEdit(this);
        postsLayout->addWidget(postEdit);
    }
}

void PostsArea::OnDeletePost(int index) {
    delete postComponents[index];
    postComponents.removeAt(index);
    for(int i=index; i<postComponents.size(); ++i)
        postComponents[i]->DecIndex();
    ui::mainWindow->RefreshUserInfoBar();
}

void PostsArea::OnAddPost(const QString &title, const QString &content) {
    //check content
    if(content.isEmpty()) {
        QMessageBox::warning(nullptr, tr("Warning"),
                             tr("Type Some Text!"), QMessageBox::Ok);
        return;
    }

    //check title
    if(title.isEmpty()) {
        QMessageBox::warning(nullptr, tr("Warning"),
                             tr("Type A Title!"), QMessageBox::Ok);
        return;
    }

    //add new post
    Post& post = Forum::Get().GetCurBoard().AddPost(title, content);
    //add new post to UI
    auto* postComponent = new PostComponent(
                post,
                postComponents.size(),
                this);//end new PostComponent()
    postComponents.append(postComponent);

    //refresh the end part of PostsArea
    delete postEdit;
    postsLayout->addWidget(postComponent);
    postEdit = new PostEdit(this);
    postsLayout->addWidget(postEdit);

    ui::mainWindow->RefreshUserInfoBar();
}

