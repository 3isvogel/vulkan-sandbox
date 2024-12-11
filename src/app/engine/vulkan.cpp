#include "vulkan.hpp"

#define x(n) &&(n##Family > -1)
bool QueueFamilyIndices::isComplete() { return true QUEUE_HANDLER_LIST; }
#undef x