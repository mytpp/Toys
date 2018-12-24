#ifndef ANONYMOUSUSER_H
#define ANONYMOUSUSER_H

#include "User/user.h"

class Anonymous : public User
{
public:
    Anonymous();

    virtual infrastructure::Profile GetProfile() override;
};

#endif // ANONYMOUSUSER_H
