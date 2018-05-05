#include <iostream>

extern "C" {
    int64_t inc(int64_t);
    void isEven(int64_t);
}

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// printd - printf that takes an integer prints it as "%lld\n", returning Void.
extern "C" DLLEXPORT void println(int64_t X) {
  std::cout << "being called externally " << X << std::endl;
//   fprintf(stderr, "%lld\n", X);
  return;
}

int main() {
    int64_t i = 10000;
    int64_t j = 5;
    // std::cout << i << " " << inc(i) << std::endl;
    // isEven(i);
    isEven(j);
    // std::cout << i << " is even "; isEven(i); std::cout << std::endl;
    // std::cout << j << " is even "; isEven(j); std::cout << std::endl;
    return 0;
}
