#include "commonuser.h"

CommonUser::CommonUser(size_t postCount)
    :postCount(postCount)
{

}

infrastructure::Profile CommonUser::GetProfile(){
    return { Id(), Name(), infrastructure::COMMON_USER, postCount };
}

void CommonUser::AddPost() {
    postCount++;
}

void CommonUser::DeletePost() {
    postCount--;
}
