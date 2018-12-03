#ifndef COMMONUSER_H
#define COMMONUSER_H

#include "User/user.h"

class CommonUser: public User
{   
public:
    CommonUser(size_t postCount = 0);

    virtual infrastructure::Profile GetProfile() override;

    virtual void AddPost() override;
    virtual void DeletePost() override;

    const size_t PostCount() const { return postCount; }

private:
    size_t postCount;
};

#endif // COMMONUSER_H
