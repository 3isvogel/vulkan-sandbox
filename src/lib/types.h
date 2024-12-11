#pragma once

#define STATUS_LIST y(OK) y(TODO) y(FAIL)
#define FAIL_LIST                                                              \
  x(SURFACE_CREATION) x(INSTANCE_CREATION) x(PHYSICAL_DEVICE)                  \
      x(LOGICAL_DEVICE) x(SWAPCHAIN) x(IMAGEVIEW) x(OPEN_FILE)                 \
          x(SHADER_MODULE)

#define y(name) name,
#define x(name) name##_FAIL,
typedef enum { STATUS_LIST FAIL_LIST STATUS_MAX } Status;
#undef y
#undef x

#define YEET(x)                                                                \
  /* Tests status of x, if it's different from OK bubbles up */                \
  do {                                                                         \
    const Status _yeet_ = x;                                                   \
    if (_yeet_ != Status::OK)                                                  \
      return _yeet_;                                                           \
  } while (0)

extern char *statuses[STATUS_MAX];

#include "log.hpp"
#ifndef BUILD_RELEASE
#define __TODO_RET_VAL Status::TODO
#else
#define __TODO_RET_VAL Status::OK
#endif
#define TODO(...)                                                              \
  /* Fails in dev with a custom message, passes silently in release */         \
  do {                                                                         \
    logInfo(SET2E(FG(BLUE_CODE), BOLD_CODE) "TODO" RESET ": " __VA_ARGS__);    \
    return __TODO_RET_VAL;                                                     \
  } while (0)