#include <lib/loader.hpp>
#include <lib/log.hpp>

// Info-1: "file_reading.cpp"
std::vector<char> readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    logError("Failed to open file: %s", filename.c_str());
    e_runtime("Failed to open file");
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}