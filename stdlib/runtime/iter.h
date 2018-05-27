//===--- iter.h - Dusk runtime iteration operations -------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_STDLIB_RUNTIME_ITER
#define DUSK_STDLIB_RUNTIME_ITER

#include <iostream>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// Takes 2 integers (start, end) and returns the size of the range.
extern "C" DLLEXPORT int64_t __iter_range(int64_t, int64_t);

/// Takes 2 integers (start, end) and returns -1, 1 which is going
/// to be a single step in range.
extern "C" DLLEXPORT int64_t __iter_step(int64_t, int64_t);

#endif /* DUSK_STDLIB_RUNTIME_ITER */
