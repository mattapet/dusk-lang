#include "dusk/Frontend/Compiler.h"
#include <iostream>

using namespace dusk;

int main(int argc, const char *argv[]) {
  std::vector<llvm::StringRef> Filenames;
  for (size_t i = 1; i < argc; i++) {
    Filenames.push_back(argv[i]);
  }
  Compiler C(Filenames);
  C.Compile();
  return 0;
}
