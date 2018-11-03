#include "user.h"
#include "Forum/forum.h"
#include "User/administrator.h"
#include "User/commonuser.h"
#include "User/moderator.h"
#include <QDebug>

User* User::_user = nullptr;

User::User()
{
}

User::~User() = default;

void User::LogIn(){
    qDebug()<<"really logged in!";
    Forum::Get().Show();
}

void User::Logout(){
    //emit signal
}

void User::SwitchToBoard(size_t index){
    //don't show "delete post"
}


infrastructure::Error User::TryLogin(QString id, QString password){
    auto response = Forum::Verify(id, password);

    if (response.error == infrastructure::SUCCESS) {
        switch (response.status) {
        case infrastructure::ADMINISTRATOR:
            _user = new Administrator();
            break;
        case infrastructure::COMMON_USER:
            _user = new CommonUser();
            break;
        case infrastructure::MODERATOR:
            _user = new Moderator();
            break;
        default:
            break;
        }
        _user->name = response.name;
        _user->password = password;
    }

    return response.error;
}

