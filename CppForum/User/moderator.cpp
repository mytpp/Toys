#include "moderator.h"

Moderator::Moderator(size_t postCount)
    :CommonUser(postCount)
{

}

infrastructure::Profile Moderator::GetProfile(){
    return { Id(), Name(), infrastructure::MODERATOR, PostCount() };
}
