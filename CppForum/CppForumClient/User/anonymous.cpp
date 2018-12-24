#include "anonymous.h"

Anonymous::Anonymous()
{

}

infrastructure::Profile Anonymous::GetProfile() {
    return { Id(), Name(), infrastructure::ANONYMOUS, 0 };
}
