#include "postsarea.h"
#include "UI/postcomponent.h"
#include "UI/postedit.h"
#include "Forum/forum.h"
#include <algorithm>
#include <QMessageBox>
#include <QUuid>
#include "Forum/post.h"
#include "User/user.h"


PostsArea::PostsArea(QWidget *parent) : QWidget(parent)
{
    postsLayout = new QVBoxLayout(this);
    postsLayout->setMargin(20);


    //retrive posts from the Forum
    auto& posts = Forum::Get().GetCurBoard().GetPosts();

    int index = 0;
    //add Posts to this area
    std::for_each(posts.begin(), posts.end(),
        [postsLayout = postsLayout, &postComponents = postComponents, &index, this]
        (const Post& post) {
        auto* postComponent = new PostComponent(post, index++, this);
        postComponents.append(postComponent);
        postsLayout->addWidget(postComponent);
    });

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
    auto guid = QUuid::createUuid().toString();
    if(Forum::Get().GetCurBoard().AddPost(guid, title, content)){
        //add new post to UI
        auto* postComponent = new PostComponent(
                    Post(guid,
                         User::Get()->Id(),
                         title,
                         content,
                         QDate::currentDate()
                         ), //end Post()
                    postComponents.size(),
                    this);//end new PostComponent()
        postComponents.append(postComponent);

        //refresh the end part of PostsArea
        delete postEdit;
        postsLayout->addWidget(postComponent);
        postEdit = new PostEdit(this);
        postsLayout->addWidget(postEdit);
    }//end if
}

