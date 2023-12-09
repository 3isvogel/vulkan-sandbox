#include <app/app.hpp>
#include <lib/log.hpp>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
  logSetLevel(LOG_LEVEL_DEBUG);
  MainApp app;

  try {
    app.run();
  } catch (const std::exception &e) {
    logError("Exception threw at %s", e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}