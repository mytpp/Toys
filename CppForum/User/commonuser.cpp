#include "commonuser.h"

CommonUser::CommonUser()
{

}

infrastructure::Profile CommonUser::GetProfile(){
    return { Id(), Name(), infrastructure::COMMON_USER, postCount };
}

void CommonUser::SwitchToBoard(size_t index) {
    //call Board's function with additional param to indicate whether to show "delete" on post
}
