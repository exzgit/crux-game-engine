#include "error.h"

void error_callback(const char* description) {
  std::cerr << "\033[31m" 
    << "==> " 
    << "error: " 
    << "\033[0m" 
    << description 
    << std::endl;
}