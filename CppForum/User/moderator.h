#ifndef MODERATOR_H
#define MODERATOR_H

#include "User/commonuser.h"

class Moderator : public CommonUser
{
public:
    Moderator();

    virtual void SwitchToBoard(size_t index) override;

private:

};

#endif // MODERATOR_H
