//===--- IRGenerator.cpp --------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/ASTVisitor.h"
#include "dusk/AST/Scope.h"
#include "dusk/AST/NameLookup.h"
#include "dusk/IRGen/IRGenerator.h"
#include "llvm/IR/BasicBlock.h"

#include "GenExpr.h"
#include "IRGenModule.h"
#include "GenModule.h"

using namespace dusk;
using namespace irgen;

IRGenerator::IRGenerator(ASTContext &C, DiagnosticEngine &Diag)
    : Context(C), Diag(Diag), Builder({LLVMContext}) {}

IRGenerator::~IRGenerator() {}

llvm::Module *IRGenerator::perform() {
  auto M = Context.getRootModule();
  Module = std::make_unique<llvm::Module>(M->getName(), LLVMContext);
  IRGenModule IRGM(Context, LLVMContext, Module.get(), Builder);
  genModule(IRGM);
  return Module.release();
}
