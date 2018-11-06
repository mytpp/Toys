#include "postsarea.h"
#include "UI/postcomponent.h"
#include "Forum/forum.h"
#include <algorithm>
#include <QMessageBox>
#include <QUuid>
#include <QGridLayout>
#include "Forum/post.h"
#include "User/user.h"
#include <QDebug>


PostsArea::PostsArea(QWidget *parent) : QScrollArea(parent)
{
    postsLayout = new QVBoxLayout(this);
    postsLayout->setMargin(20);

    if(User::Get()->GetProfile().status == infrastructure::ADMINISTRATOR){
        assignModeratorButton = new QPushButton(this);
        postsLayout->addWidget(assignModeratorButton);

        connect(assignModeratorButton, &QPushButton::clicked,[]{
            //code*****************************
        });
    }

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
        //draw the area to add new post
        newPostArea = new QFrame(this);
        titleLabel = new QLabel(tr("Title:"));
        contentEdit = new QTextEdit(newPostArea);
        contentLabel = new QLabel(tr("Content:"));
        titleEdit = new QLineEdit(newPostArea);
        postButton = new QPushButton(newPostArea);
        postButton->setText(tr("Post"));

        auto newPostLayout = new QGridLayout(newPostArea);
        newPostLayout->addWidget(titleLabel, 0, 0);
        newPostLayout->addWidget(titleEdit, 0, 1);
        newPostLayout->addWidget(contentLabel, 1, 0);
        newPostLayout->addWidget(contentEdit, 1, 1);
        newPostLayout->addWidget(postButton, 2, 1);

        newPostArea->setMinimumHeight(150);
        postsLayout->addWidget(newPostArea);

        //link postButton to its callback
        connect(postButton, &QPushButton::clicked,
                [&,contentEdit = contentEdit, titleEdit = titleEdit,
                &postComponents = postComponents, this]{

            //get post's content
            auto content = contentEdit->toPlainText();
            if(content.isEmpty()) {
                QMessageBox::warning(nullptr, tr("Warning"),
                                     tr("Type Some Text!"), QMessageBox::Ok);
                return;
            }

            //get post's title
            auto title = titleEdit->text();
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
            }//end if

        });//end lambda and connect
    }

    //add placeholder
    postsLayout->addStretch();
}

void PostsArea::OnDeletePost(int index) {
    delete postComponents[index];
    postComponents.removeAt(index);
}

