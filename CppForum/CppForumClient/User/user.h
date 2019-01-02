#ifndef USER_H
#define USER_H

#include <QString>
#include "infrastructure.h"


class User
{
public:
    User();
    virtual ~User();

    bool LogIn();
    bool Logout();

    virtual infrastructure::Profile GetProfile() =0;

    //inc postCount, if there exists one
    virtual void AddPost() {}
    virtual void DeletePost() {}

    static User* Get() { return _user; }
    static infrastructure::Error TryLogin(QString id, QString password);

    const QString& Id()   const { return id; }
    const QString& Name() const { return name; }

private:
    QString id;
    QString password;
    QString name;

    static User *_user;

};

#endif // USER_H
