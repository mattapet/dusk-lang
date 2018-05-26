//===--- io.h - Dusk runtime io operations ----------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_STDLIB_RUNTIME_IO
#define DUSK_STDLIB_RUNTIME_IO

#include <iostream>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// Prints a single integer into standart output.
extern "C" DLLEXPORT void println(int64_t);

/// Reads a single integer from the standart input and returns it.
extern "C" DLLEXPORT int64_t readln();

#endif /* DUSK_STDLIB_RUNTIME_IO */
