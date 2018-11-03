#ifndef USER_H
#define USER_H

#include <QString>
#include "infrastructure.h"


class User
{
public:
    User();
    virtual ~User();

    void LogIn();
    void Logout();

    virtual infrastructure::Profile GetProfile() =0;

    virtual void SwitchToBoard(size_t index);

    //don't let User (by virtual function) to decide whether to show
    //"delete button" or "comment button", in order to decople from
    //Forum class


    static User* Get() { return _user; }
    static infrastructure::Error TryLogin(QString id, QString password);

    const QString& Id()   const { return id; }
    const QString& Name() const { return name; }

private:
    QString id;
    QString password;
    QString name;
    //Status status;

    static User *_user;

};

#endif // USER_H
