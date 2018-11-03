#include "administrator.h"

Administrator::Administrator()
{

}

infrastructure::Profile Administrator::GetProfile(){
    return { Id(), Name(), infrastructure::ADMINISTRATOR, 0 };
}
