//===--- RuntimeFuncWrapper.h - Runtime function generation -----*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_RUNTIME_FUNC_WRAPPER_H
#define DUSK_RUNTIME_FUNC_WRAPPER_H

#include "dusk/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"

namespace dusk {
llvm::Value *getRuntimeFunc(llvm::Module *M, StringRef N,
                            ArrayRef<llvm::Type *> ArgsT, llvm::Type *RetT);
}

#endif /* DUSK_RUNTIME_FUNC_WRAPPER_H */
