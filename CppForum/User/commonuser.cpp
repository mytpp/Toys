#include "commonuser.h"
#include "Forum/forum.h"

CommonUser::CommonUser(size_t postCount)
    :postCount(postCount)
{

}

infrastructure::Profile CommonUser::GetProfile(){
    return { Id(), Name(), infrastructure::COMMON_USER, postCount };
}

void CommonUser::AddPost() {
    Forum::Get().IncPostCountOf(Id());
    postCount++;
}

void CommonUser::DeletePost() {
    Forum::Get().DecPostCountOf(Id());
    postCount--;
}
