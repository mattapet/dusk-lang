//===--- IRGenerator.cpp --------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/IRGen/IRGenerator.h"
#include "dusk/IRGen/Scope.h"

using namespace dusk;
using namespace irgen;

IRGenerator::IRGenerator(DiagnosticEngine &Diag)
    : Diag(Diag), Builder({Context}) {}

IRGenerator::~IRGenerator() { /*delete Module;*/ }

void IRGenerator::gen(ModuleDecl *D) {
  Module = llvm::make_unique<llvm::Module>(D->getName(), Context);
  wrapTopLevelExpr(codeGen(static_cast<Expr *>(D->getContents().front())));
  Module->print(llvm::errs(), nullptr);
}

llvm::Function *IRGenerator::wrapTopLevelExpr(llvm::Value *Val) {
  assert(Val && "Value cannot be NUL");

  auto Ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), false);
  auto Fn = llvm::Function::Create(Ft, llvm::Function::ExternalLinkage,
                                   "__anon_expr", Module.get());

  auto BB = llvm::BasicBlock::Create(Context, "entry", Fn);
  Builder.SetInsertPoint(BB);
  Builder.CreateRet(Val);
  llvm::verifyFunction(*Fn);
  return Fn;
}

llvm::Value *IRGenerator::codeGen(NumberLiteralExpr *E) {
  auto Value = llvm::APSInt::get(E->getValue());
  return llvm::ConstantInt::get(Context, Value);
}

llvm::Value *IRGenerator::codeGen(IdentifierExpr *E) {
  auto Value = NamedValues[E->getName()];
  // Use of undeclared variable
  if (!Value)
    return nullptr;
  return Value;
}

llvm::Value *IRGenerator::codeGen(InfixExpr *E) {
  auto LHS = codeGen(E->getLHS());
  auto RHS = codeGen(E->getRHS());
  if (!LHS || !RHS)
    return nullptr;
  
  switch (E->getOp().getKind()) {
    case tok::plus:
      return Builder.CreateAdd(LHS, RHS, "addtmp");
    case tok::minus:
      return Builder.CreateSub(LHS, RHS, "sumtmp");
    case tok::mod:
    case tok::divide:
      return Builder.CreateSDiv(LHS, RHS, "divtmp");
    case tok::multipy:
      return Builder.CreateMul(LHS, RHS, "multmp");
    case tok::equals:
      return Builder.CreateICmpEQ(LHS, RHS, "eqtmp");
    case tok::nequals:
      return Builder.CreateICmpNE(LHS, RHS, "neqtmp");
    case tok::less:
      return Builder.CreateICmpSLT(LHS, RHS);
    case tok::less_eq:
      return Builder.CreateICmpSLE(LHS, RHS);
    case tok::greater:
      return Builder.CreateICmpSGT(LHS, RHS, "greattmp");
    case tok::greater_eq:
      return Builder.CreateICmpSGE(LHS, RHS, "great_eqtmp");
    default:
      return nullptr;
  }
}

llvm::Value *IRGenerator::codeGen(PrefixExpr *E) {
  auto Value = codeGen(E->getDest());
  if (!Value)
    return nullptr;
  
  switch (E->getOp().getKind()) {
    case tok::neg:
      return Builder.CreateNot(Value);
    default:
      return nullptr;
  }
}

llvm::Value *IRGenerator::codeGen(CallExpr *E) {
  auto Callee = dynamic_cast<IdentifierExpr *>(E);
  auto Args = dynamic_cast<ExprPattern *>(E->getArgs());
  if (!Callee || !Args)
    return nullptr;
  
  auto Fn = Module->getFunction(Callee->getName());
  if (!Fn)
    return nullptr;
  
  if (Fn->arg_size() != Args->getValues().size())
    return nullptr;
  llvm::SmallVector<llvm::Value *, 128> ArgsV;
  for (unsigned i = 0; i < Args->getValues().size(); i++) {
    ArgsV.push_back(codeGen(Args->getValues()[i]));
    if (!ArgsV.back())
      return nullptr;
  }
  return Builder.CreateCall(Fn, ArgsV, "calltmp");
}

