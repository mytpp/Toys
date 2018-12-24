#include "userinfobar.h"
#include "User/user.h"
#include "Forum/forum.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QDebug>

UserInfoBar::UserInfoBar(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *userInfoLayout = new QHBoxLayout(this);

    auto profile = User::Get()->GetProfile();

    userName = new QLabel(this);
    userName->setText("Hello! " + profile.name);
    userInfoLayout->addWidget(userName);
    userInfoLayout->addSpacing(25);

    //add status info
    userStatus = new QLabel(this);
    QString status;
    switch (profile.status) {
    case infrastructure::ADMINISTRATOR: status = "ADMINISTRATOR"; break;
    case infrastructure::COMMON_USER: status = "COMMON USER"; break;
    case infrastructure::MODERATOR: status = "MODERATOR"; break;
    case infrastructure::ANONYMOUS: status = "ANONYMOUS"; break;
    default: status = "UNKNOWN"; break;
    }
    userStatus->setText(status);
    userInfoLayout->addWidget(userStatus);
    userInfoLayout->addSpacing(25);

    //conditionally add post count
    if(profile.status == infrastructure::COMMON_USER
        || profile.status == infrastructure::MODERATOR) {
        postCount = new QLabel(this);
        postCount->setText("Posts:" + QString::number(profile.postCount) );
        userInfoLayout->addWidget(postCount);
        userInfoLayout->addSpacing(25);
    }

    //separate user info from buttons
    userInfoLayout->addStretch();

    //Administrator is able to assign moderator for current board
    if(User::Get()->GetProfile().status == infrastructure::ADMINISTRATOR){
        assignButton = new QPushButton(this);
        assignButton->setText(tr("Assign Moderator"));
        userInfoLayout->addWidget(assignButton, 0, Qt::AlignRight);

        connect(assignButton, &QPushButton::clicked,
                this, &UserInfoBar::PopAssignDialog);

        dismissButton = new QPushButton(this);
        dismissButton->setText(tr("Dismiss Moderator"));
        userInfoLayout->addWidget(dismissButton, 0, Qt::AlignRight);

        connect(dismissButton, &QPushButton::clicked,
                this, &UserInfoBar::PopDismissDialog);
    }

    //add logout button
    logoutButton = new QPushButton(this);
    logoutButton->setText(tr("Log Out"));
    userInfoLayout->addWidget(logoutButton, 0, Qt::AlignRight);

    connect(logoutButton, &QPushButton::clicked, []{
        User::Get()->Logout();
    });

    this->setMaximumHeight(50);
}

void UserInfoBar::Refresh() {
    auto profile = User::Get()->GetProfile();

    userName->setText("Hello! " + profile.name);
    if(postCount)
        postCount->setText("Posts:" + QString::number(profile.postCount) );
}

void UserInfoBar::PopAssignDialog() {
    auto assignDialog = new QDialog;

    auto hintLabel = new QLabel(assignDialog);
    hintLabel->setText(tr("Please input the new moderator id for current board"));
    auto idEdit = new QLineEdit(assignDialog);

    auto okButton = new QPushButton(assignDialog);
    okButton->setText("Set");

    auto vlayout = new QVBoxLayout(assignDialog);
    vlayout->addWidget(hintLabel);
    vlayout->addWidget(idEdit);
    vlayout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, [=] {
        auto id = idEdit->text();
        if(id.isEmpty()) {
            QMessageBox::warning(nullptr, tr("Error!"),
                                 tr("User id is required!"),
                                 QMessageBox::Ok);
        } else if(Forum::Get().AssignModerator(id)) {
            QMessageBox::information(nullptr, tr("Success"),
                                 tr("Assigning Succeeds!"),
                                 QMessageBox::Ok);
        } else {
            QMessageBox::warning(nullptr, tr("Error!"),
                                 tr("Id not found!"),
                                 QMessageBox::Ok);
        }
    });

    assignDialog->show();
}

void UserInfoBar::PopDismissDialog(){
    if(Forum::Get().DismissModerator()){
        QMessageBox::information(nullptr, tr("Success"),
                                 tr("Dismissing Succeeds!"),
                                 QMessageBox::Ok);
    } else {
        QMessageBox::warning(nullptr, tr("Error!"),
                             tr("Dismissing fails!"),
                             QMessageBox::Ok);
    }
}

