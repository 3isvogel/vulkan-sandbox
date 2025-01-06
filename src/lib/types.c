#include "types.h"

#define y(name) STR2(name),
#define x(name) STR(CON(name, _FAIL)),
char *statuses[STATUS_MAX] = {STATUS_LIST FAIL_LIST};
#undef y
#undef x