#include <app/app.hpp>
#include <cstdlib>

int main() {
  logSetLevel(LOG_LEVEL_DEBUG);
  EngineApp app;

  Status status = app.run();
  if (status != Status::OK) {
    logError("Error status: %s", statuses[status]);
    return EXIT_FAILURE;
  }
  logDebug("Execution completed");
  return EXIT_SUCCESS;
}
