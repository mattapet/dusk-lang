//===--- RuntimeFuncs.h - Runtime function declarations ---------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_RUNTIME_FUNCS_H
#define DUSK_RUNTIME_FUNCS_H

#include "dusk/Basic/LLVM.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Type.h"
#include "dusk/AST/NameLookup.h"
#include "dusk/AST/ASTContext.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SourceMgr.h"
#include <memory>
#include <vector>

namespace dusk {

static ASTNode *getPrintln(ASTContext &Context) {
  auto TyRepr = std::make_unique<IdentTypeRepr>("Int");
  auto P = std::make_unique<ParamDecl>("val", SMLoc{},
                                       Context.pushTypeRepr(std::move(TyRepr)));
  llvm::SmallVector<Decl *, 128> Prms;
  Prms.push_back(Context.pushNode(std::move(P)));
  auto Pttrn = std::make_unique<VarPattern>(std::move(Prms), SMLoc{}, SMLoc{});
  auto Fn = std::make_unique<FuncDecl>("println", SMLoc{}, SMLoc{},
                                       Context.pushPattern(std::move(Pttrn)));
  auto Extrn =
      std::make_unique<ExternStmt>(SMLoc{}, Context.pushNode(std::move(Fn)));
  auto C = Context.getRootModule()->getContents();
  return Context.pushNode(std::move(Extrn));
}

static ASTNode *getReadln(ASTContext &Context) {
  NameLookup NL;
  llvm::SmallVector<Decl *, 128> Prms;
  auto Pttrn = std::make_unique<VarPattern>(std::move(Prms), SMLoc{}, SMLoc{});
  auto TyRepr = std::make_unique<IdentTypeRepr>("Int");
  auto Fn = std::make_unique<FuncDecl>("readln", SMLoc{}, SMLoc{},
                                       Context.pushPattern(std::move(Pttrn)),
                                       Context.pushTypeRepr(std::move(TyRepr)));
  auto Extrn =
      std::make_unique<ExternStmt>(SMLoc{}, Context.pushNode(std::move(Fn)));
  auto C = Context.getRootModule()->getContents();
  return Context.pushNode(std::move(Extrn));
}

static void getFuncs(ASTContext &Context) {
  std::vector<ASTNode *> NF;
  NF.push_back(getPrintln(Context));
  NF.push_back(getReadln(Context));
  auto &C = Context.getRootModule()->getContents();
  NF.insert(NF.end(), C.begin(), C.end());
  C = NF;
}

} // namespace dusk

#endif /* DUSK_RUNTIME_FUNCS_H */
