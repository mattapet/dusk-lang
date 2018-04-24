//===--- IRGenerator.h - Dusk IR generator ----------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_IR_GENERATOR_H
#define DUSK_IRGEN_IR_GENERATOR_H

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/ASTWalker.h"
#include "dusk/AST/Diagnostics.h"
#include "dusk/IRGen/Context.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <stack>
#include <memory>

namespace dusk {
namespace irgen {
  

class IRGenerator : public ASTWalker {
  llvm::StringMap<llvm::AllocaInst *> NamedValues;
  DiagnosticEngine &Diag;
  llvm::LLVMContext Ctx;
  llvm::IRBuilder<> Builder;
  std::unique_ptr<llvm::Module> Module;
  
  

public:
  IRGenerator(DiagnosticEngine &Diag);
  ~IRGenerator();
  
  bool gen(ModuleDecl *M);
  
private:
    bool prepareGlobals(Context &Ctx, ModuleDecl *M);
};

} // namespace ir

} // namespace dusk

#endif /* DUSK_IRGEN_IR_GENERATOR_H */
