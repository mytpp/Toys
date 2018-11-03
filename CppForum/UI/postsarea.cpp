#include "postsarea.h"
#include "UI/postcomponent.h"
#include "Forum/forum.h"
#include <algorithm>
#include <QMessageBox>
#include <QUuid>
#include "Forum/post.h"
#include "User/user.h"
#include <QDebug>


PostsArea::PostsArea(QWidget *parent) : QScrollArea(parent)
{
    qDebug()<<"in post area";
    auto debug = 0;
    postsLayout = new QVBoxLayout(this);
    postsLayout->setMargin(20);

    if(User::Get()->GetProfile().status == infrastructure::ADMINISTRATOR){
        assignModeratorButton = new QPushButton(this);
        postsLayout->addWidget(assignModeratorButton);

        connect(assignModeratorButton, &QPushButton::clicked,[]{});
    }

qDebug()<<"in post area"<<debug++;
    //retrive posts from the Forum
auto& f=Forum::Get();
qDebug()<<"in post area"<<debug++;
    auto& posts = f.GetCurBoard().GetPosts();
qDebug()<<"in post area"<<debug++;
    int index = 0;
    //add Posts to this area
    std::for_each(posts.begin(), posts.end(),
        [postsLayout = postsLayout, &postComponents = postComponents, &index, this]
        (const Post& post) {
        auto* postComponent = new PostComponent(post, index++, this);
        postComponents.append(postComponent);
        postsLayout->addWidget(postComponent);
    });
qDebug()<<"in post area"<<debug++;
    //check whether the new post area can be shown
    auto status = User::Get()->GetProfile().status;
    if(status == infrastructure::COMMON_USER ||
            status == infrastructure::MODERATOR) {
        //draw the area to add new post
        newPostArea = new QFrame(this);
        contentEdit = new QTextEdit(newPostArea);
        titleEdit = new QLineEdit(newPostArea);
        postButton = new QPushButton(newPostArea);
        postsLayout->addWidget(newPostArea);

qDebug()<<"in post area"<<debug++;
        //link postButton to its callback
        connect(postButton, &QPushButton::clicked,
                [&,contentEdit = contentEdit, titleEdit = titleEdit,
                &postComponents = postComponents, this]{
qDebug()<<"in post area"<<debug++;
            //get post's content
            auto content = contentEdit->toPlainText();
            if(content.isEmpty()) {
                QMessageBox::warning(nullptr, tr("Warning"),
                                     tr("Type Some Text!"), QMessageBox::Ok);
                return;
            }
qDebug()<<"in post area"<<debug++;
            //get post's title
            auto title = titleEdit->text();
            if(title.isEmpty()) {
                QMessageBox::warning(nullptr, tr("Warning"),
                                     tr("Type A Title!"), QMessageBox::Ok);
                return;
            }
qDebug()<<"in post area"<<debug++;
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
qDebug()<<"in post area"<<debug++;
        });//end lambda and connect
    }
}

void PostsArea::OnDeletePost(int index) {
    delete postComponents[index];
    postComponents.removeAt(index);
}

