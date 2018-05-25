//===--- IRGenModule.h - Module generation state ----------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_IRGEN_MODULE_H
#define DUSK_IRGEN_IRGEN_MODULE_H

#include "dusk/Basic/LLVM.h"
#include "dusk/AST/NameLookup.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/IRBuilder.h"

#include "Address.h"
#include "IRGenValue.h"

namespace llvm {
class Constant;
class Module;
class Value;
class Function;
class LLVMContext;
class StringRef;
}

namespace dusk {
class Decl;
class Expr;
class FuncDecl;
class ASTContext;

namespace irgen {

/// Main class for IR emittion of global declarations.
class IRGenModule {
public:
  ASTContext &Context;
  llvm::LLVMContext &LLVMContext;
  llvm::Module *Module;
  llvm::IRBuilder<> &Builder;

  NameLookup Lookup;
  llvm::DenseMap<Decl *, Address> Vals;

  IRGenModule(ASTContext &Ctx, llvm::LLVMContext &LLVMCtx, llvm::Module *M,
              llvm::IRBuilder<> &B);

  /// Declares a global variable
  Address declareVal(Decl *D);
  /// Decalres a fuction.
  Address declareFunc(FuncDecl *D);

  /// Returns value of declared variable.
  Address getVal(StringRef N);
  /// Returns declared function.
  llvm::Function *getFunc(StringRef N);

  /// Emits an R value.
  RValue emitRValue(Expr *E);
  /// Emits an L value.
  LValue emitLValue(Expr *E);

  RValue emitRValue(Type *Ty);
};

} // namespace irgen
} // namespace dusk

#endif /* DUSK_IRGEN_IRGEN_MODULE_H */
