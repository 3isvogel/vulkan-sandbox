#include "loader.hpp"
#include "log.hpp"
#include <fstream>

Status readFile(Bytes &buffer, String filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    logDebug("Could not open: %s", filename.c_str());
    return Status::OPEN_FILE_FAIL;
  }

  size_t fileSize = (size_t)file.tellg();
  buffer.resize(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return Status::OK;
}
