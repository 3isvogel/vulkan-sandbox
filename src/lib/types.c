#include "types.h"
#include "macros.hpp"

#define y(name) #name,
#define x(name) STRINGIZE2(name##_FAIL),
char *statuses[STATUS_MAX] = {STATUS_LIST FAIL_LIST};
#undef y
#undef x