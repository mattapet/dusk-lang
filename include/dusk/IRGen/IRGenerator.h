//===--- IRGenerator.h - Dusk IR generator ----------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IR_GENERATOR_H
#define DUSK_IR_GENERATOR_H

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/ASTWalker.h"
#include "dusk/AST/Diagnostics.h"
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
  llvm::LLVMContext Context;
  llvm::IRBuilder<> Builder;
  std::unique_ptr<llvm::Module> Module;

public:
  IRGenerator(DiagnosticEngine &Diag);
  ~IRGenerator();
  
  void gen(ModuleDecl *Module);
  
private:
  
  
  llvm::Value *codeGen(Expr *E) {
    switch (E->getKind()) {
      case ExprKind::NumberLiteral:
        return codeGen(static_cast<NumberLiteralExpr *>(E));
      case ExprKind::Identifier:
        return codeGen(static_cast<IdentifierExpr *>(E));
      case ExprKind::Paren:
        return codeGen(static_cast<ParenExpr *>(E)->getExpr());
      case ExprKind::Infix:
        return codeGen(static_cast<InfixExpr *>(E));
        //  case ExprKind::Assign:
        //    return codeGen(static_cast<AssignExpr *>(E));
      case ExprKind::Prefix:
        return codeGen(static_cast<PrefixExpr *>(E));
      case ExprKind::Call:
        return codeGen(static_cast<CallExpr *>(E));
      default:
        llvm_unreachable("Unsupported expression");
    }
  }
  
  llvm::Function *wrapTopLevelExpr(llvm::Value *Val);
  llvm::Value *codeGen(NumberLiteralExpr *E);
  llvm::Value *codeGen(IdentifierExpr *E);
  llvm::Value *codeGen(InfixExpr *E);
  llvm::Value *codeGen(PrefixExpr *E);
  llvm::Value *codeGen(CallExpr *E);
};

} // namespace ir

} // namespace dusk

#endif /* DUSK_IR_GENERATOR_H */
