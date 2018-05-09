#include <iostream>

extern "C" {
    int64_t inc(int64_t);
    int64_t isEven(int64_t);
    void main_();
}

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// printd - printf that takes an integer prints it as "%lld\n", returning Void.
extern "C" DLLEXPORT void println(int64_t X) {
  fprintf(stdout, "%lld\n", X);
  return;
}

/// printd - printf that takes an integer prints it as "%lld\n", returning Void.
extern "C" DLLEXPORT int64_t readln() {
  int64_t X;
  fscanf(stdin, "%lld", &X);
  return X;
}

// int main() {
//     int64_t i = 10000;
//     int64_t j = 5;
//     // std::cout << i << " " << inc(i) << std::endl;
//     // isEven(i);
//     // std::cout << i << " is even " << isEven(i) << std::endl;
//     // std::cout << j << " is even " << isEven(j) << std::endl;
//     // isEven(j);
//     main_();
//     return 0;
// }
