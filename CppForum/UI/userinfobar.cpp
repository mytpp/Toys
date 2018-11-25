#include "userinfobar.h"
#include "User/user.h"
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QDebug>

UserInfoBar::UserInfoBar(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *userInfoLayout = new QHBoxLayout(this);

    auto profile = User::Get()->GetProfile();

//    userId = new QLabel(this);
//    userId->setText(profile.id);
//    userInfoLayout->addWidget(userId);

    userName = new QLabel(this);
    userName->setText("Hello! " + profile.name);
    userInfoLayout->addWidget(userName);

    //add status info
    userStatus = new QLabel(this);
    QString status;
    switch (profile.status) {
    case infrastructure::ADMINISTRATOR: status = "ADMINISTRATOR"; break;
    case infrastructure::COMMON_USER: status = "COMMON USER"; break;
    case infrastructure::MODERATOR: status = "MODERATOR"; break;
    default: status = "UNKNOWN"; break;
    }
    userStatus->setText(status);
    userInfoLayout->addWidget(userStatus);

    //conditionally add post count
    if(profile.status != infrastructure::ADMINISTRATOR) {
        postCount = new QLabel(this);
        postCount->setText("Posts:" + QString::number(profile.postCount) );
        userInfoLayout->addWidget(postCount);
    }

    //add logout button
    logoutButton = new QPushButton(this);
    logoutButton->setText(tr("Log Out"));
    userInfoLayout->addSpacing(100);
    userInfoLayout->addWidget(logoutButton, 0, Qt::AlignRight);

    connect(logoutButton, &QPushButton::clicked, []{
        User::Get()->Logout();
    });

    this->setMaximumHeight(50);
}

void UserInfoBar::Refresh() {
    auto profile = User::Get()->GetProfile();

    userName->setText("Hello! " + profile.name);
}
