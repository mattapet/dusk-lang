//===--- io.cpp -----------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "io.h"

void println(int64_t Value) {
  fprintf(stdout, "%lld\n", Value);
  return;
}

int64_t readln() {
  int64_t Value;
  fscanf(stdin, "%lld", &Value);
  return Value;
}
