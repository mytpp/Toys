#ifndef MODERATOR_H
#define MODERATOR_H

#include "User/commonuser.h"

class Moderator : public CommonUser
{
public:
    Moderator(size_t postCount = 0);

    virtual void SwitchToBoard(size_t index) override;

private:

};

#endif // MODERATOR_H
