// Ideally only include core.h to include all libraries in lib
#include <lib/log.hpp>

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define _EX_ __FILE__ ":" STRINGIZE(__LINE__) ": "