//===--- iter.cpp ---------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "iter.h"

#include <iostream>

int64_t __iter_range(int64_t Start, int64_t End) {
  auto Diff = (Start > 0 && End < 0) || (Start < 0 && End > 0)
    ? std::abs(Start) + std::abs(End) : Start - End;
  return std::abs(Diff);
}

int64_t __iter_step(int64_t Start, int64_t End) {
  return Start < End ? 1 : -1;
}
