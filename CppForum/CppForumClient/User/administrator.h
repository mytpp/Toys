#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include "User/user.h"

class Administrator: public User
{
public:
    Administrator();

    virtual infrastructure::Profile GetProfile() override;
};

#endif // ADMINISTRATOR_H
